#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#define _POSIX_C_SOURCE 200112L

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HOST_SIZE 256
#define PORT_SIZE 16

// TCP struct, fd, host, port
typedef struct {
  int fd;
  char host[HOST_SIZE];
  char port[PORT_SIZE];
} tcp_client;

// tcp_client_init
int tcp_client_init(tcp_client *self, const char *host, const char *port);
// tcp_client_connect
int tcp_client_connect(tcp_client *self);
// tcp_client_read
int tcp_client_read(tcp_client *self, void *buf, size_t len);
// tcp_client_write
int tcp_client_write(tcp_client *self, const void *buf, size_t len);
// tcp_client_disconnect
void tcp_client_disconnect(tcp_client *self);
// tcp_client_dispose
void tcp_client_dispose(tcp_client *self);

#endif
