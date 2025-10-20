#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libs/tcp.h"

int main(int argc, char **argv) {
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : 8080;

    int fd = tcp_connect(host, port);
    if (fd < 0) {
        fprintf(stderr, "connect failed\n");
        return 1;
    }

    const char *req =
        "GET /weather HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n";
    tcp_write_all(fd, req, strlen(req));

    char buf[8192];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = 0;
        printf("%s", buf);
    }
    close(fd);
    return 0;
}