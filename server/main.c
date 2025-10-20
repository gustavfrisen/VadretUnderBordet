// Standard C libraries for I/O, strings, memory, and system calls
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
// Custom libraries for TCP networking and HTTP protocol handling
#include "../libs/tcp.h"
#include "../libs/http.h"

int main(int argc, char **argv) {
    // Set port: use command line argument or default to 8080
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    
    // Create TCP server socket listening on specified port with backlog of 16
    int lfd = tcp_listen(port, 16);
    if (lfd < 0) {
        perror("listen");  // Print error if socket creation fails
        return 1;
    }
    printf("Listening on %d\n", port);  // Confirm server is running

    // Main server loop - runs forever accepting connections
    while (1) {
        // Accept incoming client connection (blocks until client connects)
        int cfd = tcp_accept(lfd);
        if (cfd < 0) continue;  // Skip if connection failed

        // Buffer to store incoming HTTP request data
        char inbuf[4096];
        // Read request data from client socket
        ssize_t r = read(cfd, inbuf, sizeof(inbuf)-1);
        if (r <= 0) { close(cfd); continue; }  // Close if no data received
        inbuf[r] = 0;  // Null-terminate the received string

        // Parse the raw HTTP request into structured format
        http_request_t req;
        if (http_parse_request(inbuf, (size_t)r, &req) != 0) {
            // If parsing failed, send error response
            static char out[4096];
            http_build_response(out, sizeof(out), "{\"error\":\"bad request\"}\n", "application/json", 400);
            (void)write(cfd, out, strlen(out));  // Send error to client
            close(cfd);  // Close connection
            continue;    // Process next connection
        }

        // Prepare successful weather response with JSON data
        const char *body = "{\"weather\":\"sunny\",\"temp\":20}\n";
        static char out[4096];
        // Build complete HTTP response with headers and JSON body
        http_build_response(out, sizeof(out), body, "application/json", 200);
        (void)write(cfd, out, strlen(out));  // Send response to client
        close(cfd);  // Close connection (HTTP/1.1 with Connection: close)
    }
}