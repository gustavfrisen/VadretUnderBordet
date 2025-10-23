#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TCPClient.h"

#define MAX_CLIENTS 10
#define SERVER_BUFFER_SIZE 512

typedef struct {
  int listen_fd;
  tcp_client clients[MAX_CLIENTS];
} tcp_server;

int tcp_server_initiate(tcp_server *self, const char *port);
int tcp_server_accept(tcp_server *self);
void tcp_server_work(tcp_server *self);
void tcp_server_dispose(tcp_server *self);

static inline int tcp_server_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0)
    return -1;

  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#endif