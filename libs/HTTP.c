#include "HTTP.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

static int connect_to_host(const char* host, int port);

static int parse_url(const char* url, char* host, char* path, int* port, int* is_https) {
    *is_https = 0;
    *port = 80;
    
    if (strncmp(url, "https://", 8) == 0) {
        *is_https = 1;
        *port = 443;
        url += 8;
    } else if (strncmp(url, "http://", 7) == 0) {
        url += 7;
    } else {
        return -1;
    }
    
    const char* slash = strchr(url, '/');
    const char* colon = strchr(url, ':');
    
    if (colon && (!slash || colon < slash)) {
        size_t host_len = colon - url;
        strncpy(host, url, host_len);
        host[host_len] = '\0';
        *port = atoi(colon + 1);
        
        if (slash) {
            strcpy(path, slash);
        } else {
            strcpy(path, "/");
        }
    } else {
        if (slash) {
            size_t host_len = slash - url;
            strncpy(host, url, host_len);
            host[host_len] = '\0';
            strcpy(path, slash);
        } else {
            strcpy(host, url);
            strcpy(path, "/");
        }
    }
    
    return 0;
}

static int ssl_initialized = 0;

static void init_ssl() {
    if (!ssl_initialized) {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        ssl_initialized = 1;
    }
}

static SSL_CTX* create_ssl_context() {
    const SSL_METHOD* method;
    SSL_CTX* ctx;
    
    init_ssl();
    
    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        return NULL;
    }
    
    return ctx;
}

static int connect_to_host_ssl(const char* host, int port, SSL** ssl_out) {
    int sockfd;
    SSL_CTX* ctx;
    SSL* ssl;
    
    sockfd = connect_to_host(host, port);
    if (sockfd == -1) {
        return -1;
    }
    
    ctx = create_ssl_context();
    if (!ctx) {
        close(sockfd);
        return -1;
    }
    
    ssl = SSL_new(ctx);
    if (!ssl) {
        SSL_CTX_free(ctx);
        close(sockfd);
        return -1;
    }
    
    SSL_set_fd(ssl, sockfd);
    
    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sockfd);
        return -1;
    }
    
    *ssl_out = ssl;
    return sockfd;
}

static char* read_response_ssl(SSL* ssl) {
    char buffer[4096];
    char* response = malloc(1);
    response[0] = '\0';
    size_t total_size = 0;
    int bytes_received;
    
    while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';
        
        char* new_response = realloc(response, total_size + bytes_received + 1);
        if (!new_response) {
            free(response);
            return NULL;
        }
        response = new_response;
        
        strcat(response, buffer);
        total_size += bytes_received;
        
        if (strstr(response, "\r\n\r\n")) {
            char* body_start = strstr(response, "\r\n\r\n") + 4;
            if (strstr(response, "Content-Length:")) {
                char* content_length_str = strstr(response, "Content-Length:") + 15;
                int content_length = atoi(content_length_str);
                int body_length = strlen(body_start);
                
                if (body_length >= content_length) {
                    break;
                }
            } else if (strstr(response, "Transfer-Encoding: chunked")) {
                if (strstr(body_start, "\r\n0\r\n\r\n")) {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    return response;
}

static int connect_to_host(const char* host, int port) {
    struct hostent* he;
    struct sockaddr_in server_addr;
    int sockfd;
    
    if ((he = gethostbyname(host)) == NULL) {
        return -1;
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)he->h_addr_list[0]);
    memset(&(server_addr.sin_zero), 0, 8);
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

static char* read_response(int sockfd) {
    char buffer[4096];
    char* response = malloc(1);
    response[0] = '\0';
    size_t total_size = 0;
    ssize_t bytes_received;
    
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        char* new_response = realloc(response, total_size + bytes_received + 1);
        if (!new_response) {
            free(response);
            return NULL;
        }
        response = new_response;
        
        strcat(response, buffer);
        total_size += bytes_received;
        
        if (strstr(response, "\r\n\r\n")) {
            char* body_start = strstr(response, "\r\n\r\n") + 4;
            if (strstr(response, "Content-Length:")) {
                char* content_length_str = strstr(response, "Content-Length:") + 15;
                int content_length = atoi(content_length_str);
                int body_length = strlen(body_start);
                
                if (body_length >= content_length) {
                    break;
                }
            } else if (strstr(response, "Transfer-Encoding: chunked")) {
                if (strstr(body_start, "\r\n0\r\n\r\n")) {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    return response;
}

static http_response_t* parse_http_response(const char* raw_response) {
    if (!raw_response) return NULL;
    
    http_response_t* response = malloc(sizeof(http_response_t));
    if (!response) return NULL;
    
    response->status_code = 0;
    response->body = NULL;
    response->body_length = 0;
    
    // Make a copy for parsing status line (since strtok modifies the string)
    size_t response_len = strlen(raw_response);
    char* response_copy = malloc(response_len + 1);
    if (!response_copy) {
        free(response);
        return NULL;
    }
    strcpy(response_copy, raw_response);
    
    // Parse status line
    char* status_line = strtok(response_copy, "\r\n");
    if (status_line) {
        sscanf(status_line, "HTTP/%*s %d", &response->status_code);
    }
    
    // Find body start in the original string (not the modified copy)
    char* body_start = strstr(raw_response, "\r\n\r\n");
    if (body_start) {
        body_start += 4; // Skip the "\r\n\r\n"
        
        if (strstr(raw_response, "Transfer-Encoding: chunked")) {
            // Handle chunked encoding
            char* decoded_body = malloc(strlen(body_start) + 1);
            if (!decoded_body) {
                free(response_copy);
                free(response);
                return NULL;
            }
            
            char* write_pos = decoded_body;
            char* read_pos = body_start;
            
            while (*read_pos) {
                char* line_end = strstr(read_pos, "\r\n");
                if (!line_end) break;
                
                *line_end = '\0';
                int chunk_size = strtol(read_pos, NULL, 16);
                *line_end = '\r';
                
                if (chunk_size == 0) break;
                
                read_pos = line_end + 2;
                memcpy(write_pos, read_pos, chunk_size);
                write_pos += chunk_size;
                read_pos += chunk_size + 2;
            }
            
            *write_pos = '\0';
            response->body_length = write_pos - decoded_body;
            response->body = decoded_body;
        } else {
            // Handle regular content
            response->body_length = strlen(body_start);
            response->body = malloc(response->body_length + 1);
            if (response->body) {
                strcpy(response->body, body_start);
            }
        }
    }
    
    free(response_copy);
    return response;
}

int http_get(const char *url, http_response_t **response) {
    if (!url || !response) return -1;
    
    char host[256];
    char path[1024];
    int port;
    int is_https;
    
    if (parse_url(url, host, path, &port, &is_https) != 0) {
        return -1;
    }
    
    char request[2048];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: SimpleHTTPClient/1.0\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host);
    
    char* raw_response = NULL;
    
    if (is_https) {
        SSL* ssl;
        int sockfd = connect_to_host_ssl(host, port, &ssl);
        if (sockfd == -1) {
            return -1;
        }
        
        if (SSL_write(ssl, request, strlen(request)) <= 0) {
            SSL_CTX* ctx = SSL_get_SSL_CTX(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sockfd);
            return -1;
        }
        
        raw_response = read_response_ssl(ssl);
        SSL_CTX* ctx = SSL_get_SSL_CTX(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sockfd);
    } else {
        int sockfd = connect_to_host(host, port);
        if (sockfd == -1) {
            return -1;
        }
        
        if (send(sockfd, request, strlen(request), 0) == -1) {
            close(sockfd);
            return -1;
        }
        
        raw_response = read_response(sockfd);
        close(sockfd);
    }
    
    if (!raw_response) {
        return -1;
    }
    
    *response = parse_http_response(raw_response);
    free(raw_response);
    
    return *response ? 0 : -1;
}

int http_post(const char *url, const char *body, http_response_t **response) {
    if (!url || !body || !response) return -1;
    
    char host[256];
    char path[1024];
    int port;
    int is_https;
    
    if (parse_url(url, host, path, &port, &is_https) != 0) {
        return -1;
    }
    
    char request[4096];
    snprintf(request, sizeof(request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: SimpleHTTPClient/1.0\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, host, strlen(body), body);
    
    char* raw_response = NULL;
    
    if (is_https) {
        SSL* ssl;
        int sockfd = connect_to_host_ssl(host, port, &ssl);
        if (sockfd == -1) {
            return -1;
        }
        
        if (SSL_write(ssl, request, strlen(request)) <= 0) {
            SSL_CTX* ctx = SSL_get_SSL_CTX(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(sockfd);
            return -1;
        }
        
        raw_response = read_response_ssl(ssl);
        SSL_CTX* ctx = SSL_get_SSL_CTX(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sockfd);
    } else {
        int sockfd = connect_to_host(host, port);
        if (sockfd == -1) {
            return -1;
        }
        
        if (send(sockfd, request, strlen(request), 0) == -1) {
            close(sockfd);
            return -1;
        }
        
        raw_response = read_response(sockfd);
        close(sockfd);
    }
    
    if (!raw_response) {
        return -1;
    }
    
    *response = parse_http_response(raw_response);
    free(raw_response);
    
    return *response ? 0 : -1;
}

int http_response_free(http_response_t *response) {
    if (!response) return -1;
    
    if (response->body) {
        free(response->body);
    }
    free(response);
    
    return 0;
}

int http_response_get_status(const http_response_t *response) {
    if (!response) return -1;
    return response->status_code;
}

int http_response_get_body(const http_response_t *response, char **body) {
    if (!response || !body) return -1;
    
    if (response->body) {
        *body = malloc(response->body_length + 1);
        if (!*body) return -1;
        strcpy(*body, response->body);
        return 0;
    }
    
    *body = NULL;
    return -1;
}
