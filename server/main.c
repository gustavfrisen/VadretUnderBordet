#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libs/tcp.h"
#include "../libs/http.h"

int main(int argc, char **argv) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    int lfd = tcp_listen(port, 16);
    if (lfd < 0) {
        perror("listen");
        return 1;
    }
    printf("Listening on %d\n", port);

    while (1) {
        int cfd = tcp_accept(lfd);
        if (cfd < 0) continue;

        char inbuf[4096];
        ssize_t r = read(cfd, inbuf, sizeof(inbuf)-1);
        if (r <= 0) { close(cfd); continue; }
        inbuf[r] = 0;

        http_request_t req;
        if (http_parse_request(inbuf, (size_t)r, &req) != 0) {
            static char out[4096];
            http_build_response(out, sizeof(out), "{\"error\":\"bad request\"}\n", "application/json", 400);
            (void)write(cfd, out, strlen(out));
            close(cfd);
            continue;
        }

        const char *body = "{\"weather\":\"sunny\",\"temp\":20}\n";
        static char out[4096];
        http_build_response(out, sizeof(out), body, "application/json", 200);
        (void)write(cfd, out, strlen(out));
        close(cfd);
    }
}