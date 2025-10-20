// Header guard prevents multiple inclusions of this file
#ifndef TCP_H
#define TCP_H

// Standard libraries for size definitions and data types
#include <stddef.h>      // For size_t type
#include <sys/types.h>   // For ssize_t type

// TCP Server Functions
int tcp_listen(int port, int backlog);        // Create listening socket on port with connection queue size

// TCP Connection Functions  
int tcp_accept(int listen_fd);                // Accept incoming connection, returns client socket
int tcp_connect(const char *host, int port);  // Connect to server at host:port, returns socket

// TCP Data Transfer Functions
ssize_t tcp_read_all(int fd, char *buf, size_t maxlen);      // Read data from socket into buffer
ssize_t tcp_write_all(int fd, const char *buf, size_t len);  // Write data from buffer to socket

#endif // TCP_H