// Header guard prevents multiple inclusions of this file
#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>  // For size_t type definition

// Structure to store parsed HTTP request components
typedef struct {
    char method[8];    // HTTP method (GET, POST, etc.) - max 7 chars + null terminator
    char path[256];    // Request path/URI (e.g., "/weather") - max 255 chars + null terminator  
    char version[16];  // HTTP version (e.g., "HTTP/1.1") - max 15 chars + null terminator
} http_request_t;

// HTTP Request Processing Functions
int http_parse_request(const char *raw, size_t len, http_request_t *req);  // Parse raw HTTP request into struct
int http_build_response(char *out, size_t out_sz, const char *body, const char *content_type, int status);  // Build complete HTTP response

#endif // HTTP_H