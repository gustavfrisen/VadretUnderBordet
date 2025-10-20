// TCP networking implementation - handles low-level socket operations
#include "tcp.h"
// Standard libraries for system calls, networking, and string operations
#include <stdio.h>       // For snprintf
#include <string.h>      // For memset
#include <unistd.h>      // For read, write, close
#include <errno.h>       // For error handling
#include <netdb.h>       // For getaddrinfo, freeaddrinfo
#include <sys/types.h>   // For data types
#include <sys/socket.h>  // For socket functions
#include <netinet/in.h>  // For sockaddr_in structure
#include <arpa/inet.h>   // For network address conversion
#include <fcntl.h>       // For file control operations

// Creates a TCP server socket that listens for incoming connections
int tcp_listen(int port, int backlog) {
    // Create TCP socket (AF_INET = IPv4, SOCK_STREAM = TCP)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;  // Return error if socket creation failed

    // Set SO_REUSEADDR to allow immediate reuse of port after server restart
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address structure
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));           // Clear structure
    addr.sin_family = AF_INET;                // IPv4 address family
    addr.sin_addr.s_addr = INADDR_ANY;        // Accept connections from any IP
    addr.sin_port = htons((uint16_t)port);    // Convert port to network byte order

    // Bind socket to specified port on all available interfaces
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);  // Cleanup on failure
        return -1;
    }
    // Start listening for connections with specified backlog queue size
    if (listen(fd, backlog) < 0) {
        close(fd);  // Cleanup on failure
        return -1;
    }
    return fd;  // Return listening socket descriptor
}

// Accepts an incoming connection on a listening socket
int tcp_accept(int listen_fd) {
    // Accept connection (NULL = don't store client address info)
    return accept(listen_fd, NULL, NULL);
}

// Establishes a TCP connection to a remote server
int tcp_connect(const char *host, int port) {
    // Convert port number to string for getaddrinfo
    char portstr[16];
    snprintf(portstr, sizeof(portstr), "%d", port);

    // Configure address resolution hints
    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));      // Clear hints structure
    hints.ai_family = AF_UNSPEC;           // Allow both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;       // TCP socket type

    // Resolve hostname/IP and port to network addresses
    if (getaddrinfo(host, portstr, &hints, &res) != 0) return -1;

    int fd = -1;
    // Try connecting to each resolved address until one succeeds
    for (struct addrinfo *p = res; p; p = p->ai_next) {
        // Create socket for this address family/type
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) continue;  // Try next address if socket creation failed
        
        // Attempt connection to this address
        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) break;  // Success!
        
        close(fd);  // Connection failed, cleanup and try next address
        fd = -1;
    }
    freeaddrinfo(res);  // Free memory allocated by getaddrinfo
    return fd;          // Return connected socket or -1 on failure
}

// Reads data from socket, continuing until maxlen bytes read or connection closes
ssize_t tcp_read_all(int fd, char *buf, size_t maxlen) {
    ssize_t n = 0, r;  // n = total bytes read, r = bytes from last read call
    
    // Keep reading until buffer full or no more data available
    while (n < (ssize_t)maxlen && (r = read(fd, buf + n, maxlen - n)) > 0) n += r;
    return n;  // Return total bytes successfully read
}

// Writes all data to socket, handling partial writes
ssize_t tcp_write_all(int fd, const char *buf, size_t len) {
    size_t n = 0;  // Track total bytes written
    
    // Keep writing until all data sent
    while (n < len) {
        // Write remaining data starting from current position
        ssize_t w = write(fd, buf + n, len - n);
        if (w <= 0) return -1;  // Return error if write failed
        n += (size_t)w;         // Update bytes written counter
    }
    return (ssize_t)n;  // Return total bytes successfully written
}