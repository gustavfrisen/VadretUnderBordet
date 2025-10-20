#include "http.h"
#include <string.h>
#include <stdio.h>

static const char *status_text(int status) {
    switch (status) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        default: return "OK";
    }
}

int http_parse_request(const char *raw, size_t len, http_request_t *req) {
    if (len < 14) return -1;
    const char *sp1 = strchr(raw, ' ');
    if (!sp1) return -1;
    size_t mlen = sp1 - raw;
    if (mlen >= sizeof(req->method)) mlen = sizeof(req->method)-1;
    memcpy(req->method, raw, mlen);
    req->method[mlen] = 0;

    const char *sp2 = strchr(sp1+1, ' ');
    if (!sp2) return -1;
    size_t plen = sp2 - (sp1+1);
    if (plen >= sizeof(req->path)) plen = sizeof(req->path)-1;
    memcpy(req->path, sp1+1, plen);
    req->path[plen] = 0;

    const char *crlf = strstr(sp2+1, "\r\n");
    if (!crlf) return -1;
    size_t vlen = crlf - (sp2+1);
    if (vlen >= sizeof(req->version)) vlen = sizeof(req->version)-1;
    memcpy(req->version, sp2+1, vlen);
    req->version[vlen] = 0;
    return 0;
}

int http_build_response(char *out, size_t out_sz, const char *body, const char *content_type, int status) {
    int n = snprintf(out, out_sz,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n%s",
        status, status_text(status), content_type, strlen(body), body);
    return (n < 0 || (size_t)n >= out_sz) ? -1 : n;
}