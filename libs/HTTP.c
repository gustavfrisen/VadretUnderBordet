// HTTP protocol implementation - handles parsing requests and building responses
#include "http.h"
#include <string.h>  // For string manipulation functions
#include <stdio.h>   // For snprintf

// Helper function to convert HTTP status codes to text descriptions
static const char *status_text(int status) {
    switch (status) {
        case 200: return "OK";           // Success response
        case 400: return "Bad Request";  // Client error - malformed request
        case 404: return "Not Found";    // Requested resource not found
        default: return "OK";            // Default to OK for unknown codes
    }
}

// Parses raw HTTP request string into structured format
// Expected format: "METHOD /path HTTP/version\r\n..."
int http_parse_request(const char *raw, size_t len, http_request_t *req) {
    // Minimum valid request: "GET / HTTP/1.1\r\n" = 14 characters
    if (len < 14) return -1;
    
    // Find first space to separate method from path
    const char *sp1 = strchr(raw, ' ');
    if (!sp1) return -1;  // Invalid request format
    
    // Extract HTTP method (GET, POST, etc.)
    size_t mlen = sp1 - raw;  // Calculate method length
    if (mlen >= sizeof(req->method)) mlen = sizeof(req->method)-1;  // Prevent buffer overflow
    memcpy(req->method, raw, mlen);  // Copy method to struct
    req->method[mlen] = 0;           // Null-terminate string

    // Find second space to separate path from version
    const char *sp2 = strchr(sp1+1, ' ');
    if (!sp2) return -1;  // Invalid request format
    
    // Extract request path/URI
    size_t plen = sp2 - (sp1+1);  // Calculate path length
    if (plen >= sizeof(req->path)) plen = sizeof(req->path)-1;  // Prevent buffer overflow
    memcpy(req->path, sp1+1, plen);  // Copy path to struct
    req->path[plen] = 0;             // Null-terminate string

    // Find end of request line (marked by \r\n)
    const char *crlf = strstr(sp2+1, "\r\n");
    if (!crlf) return -1;  // Invalid request format
    
    // Extract HTTP version
    size_t vlen = crlf - (sp2+1);  // Calculate version length
    if (vlen >= sizeof(req->version)) vlen = sizeof(req->version)-1;  // Prevent buffer overflow
    memcpy(req->version, sp2+1, vlen);  // Copy version to struct
    req->version[vlen] = 0;             // Null-terminate string
    
    return 0;  // Success
}

// Builds complete HTTP response with headers and body
int http_build_response(char *out, size_t out_sz, const char *body, const char *content_type, int status) {
    // Format complete HTTP response with required headers
    int n = snprintf(out, out_sz,
        "HTTP/1.1 %d %s\r\n"          // Status line (version, code, text)
        "Content-Type: %s\r\n"        // MIME type of response body
        "Content-Length: %zu\r\n"     // Size of response body in bytes
        "Connection: close\r\n"       // Close connection after response
        "\r\n%s",                     // Empty line separates headers from body
        status, status_text(status), content_type, strlen(body), body);
    
    // Return error if formatting failed or output buffer too small
    return (n < 0 || (size_t)n >= out_sz) ? -1 : n;
}