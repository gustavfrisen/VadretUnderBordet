// Standard C libraries for I/O, strings, memory, and system calls
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
// Custom TCP networking library for socket operations
#include "../libs/tcp.h"

int main(int argc, char **argv) {
    // Set server address: use command line argument or default to localhost
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    // Set server port: use command line argument or default to 8080
    int port = (argc > 2) ? atoi(argv[2]) : 8080;

    // Establish TCP connection to weather server
    int fd = tcp_connect(host, port);
    if (fd < 0) {
        fprintf(stderr, "connect failed\n");  // Print error if connection fails
        return 1;
    }

    // Construct HTTP GET request for weather data
    // \r\n = carriage return + line feed (HTTP line endings)
    // Empty line (\r\n\r\n) signals end of HTTP headers
    const char *req =
        "GET /weather HTTP/1.1\r\n"      // HTTP method, path, and version
        "Host: localhost\r\n"            // Required HTTP/1.1 host header
        "Connection: close\r\n"          // Close connection after response
        "\r\n";                          // Empty line ends HTTP request
    
    // Send complete HTTP request to server
    tcp_write_all(fd, req, strlen(req));

    // Buffer to store server's HTTP response
    char buf[8192];
    // Read server response (headers + JSON body)
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = 0;        // Null-terminate received data
        printf("%s", buf); // Print complete HTTP response to console
    }
    close(fd);  // Close connection to server
    return 0;   // Exit successfully
}