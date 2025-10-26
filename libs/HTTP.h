#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct http_response_t {
    int status_code;
    char* body;
    size_t body_length;
} http_response_t;

typedef struct http_request_t {
    char* url;
    char* method;
    char* body;
} http_request_t;

int http_get(const char* url, http_response_t** response);
int http_response_free(http_response_t* response);
int http_response_get_status(const http_response_t* response);
int http_response_get_body(const http_response_t* response, char** body);

int http_post(const char* url, const char* body, http_response_t** response);

#endif