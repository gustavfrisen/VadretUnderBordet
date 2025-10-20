#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct {
    char method[8];
    char path[256];
    char version[16];
} http_request_t;

int http_parse_request(const char *raw, size_t len, http_request_t *req);
int http_build_response(char *out, size_t out_sz, const char *body, const char *content_type, int status);
#endif