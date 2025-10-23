#include "TCPServer.h"
#include <errno.h>

int tcp_server_initiate(tcp_server *self, const char *port) {
  if (self == NULL) {
    return -1;
  }
  struct addrinfo hints = {0}, *res = NULL;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port, &hints, &res) != 0) {
    return -1;
  }

  int fd = -1;
  for (struct addrinfo *rp = res; rp; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (fd < 0) {
      continue;
    }

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;
    }

    close(fd);
    fd = -1;
  }

  freeaddrinfo(res);
  if (fd < 0) {
    printf("Failed to initialize host");
    return -1;
  }

  if (listen(fd, MAX_CLIENTS) < 0) {
    close(fd);
    return -1;
  }

  tcp_server_nonblocking(fd);

  self->listen_fd = fd;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    self->clients[i].fd = -1;
  }

  return 0;
}

void tcp_server_dispose(tcp_server *self) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    tcp_client_dispose(&self->clients[i]);
  }
  if (self->listen_fd >= 0) {
    close(self->listen_fd);
  }
  self->listen_fd = -1;
}

int tcp_server_accept(tcp_server *self) {
  int client_fd = accept(self->listen_fd, NULL, NULL);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
    return -1;
  }

  tcp_server_nonblocking(client_fd);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (self->clients[i].fd < 0) {
      self->clients[i].fd = client_fd;
      printf("Client #%d connected\n", i);
      //+1 client
      return 1;
    }
  }

  // max clients
  close(client_fd);
  return 0;
}

void tcp_server_work(tcp_server *self) {
  tcp_server_accept(self);

  char buf[SERVER_BUFFER_SIZE];

  for (int i = 0; i < MAX_CLIENTS; i++) {
    tcp_client *client = &self->clients[i];
    if (client->fd < 0) {
      continue;
    }
    size_t n = tcp_client_read(client, buf, sizeof(buf));

    if (n > 0) {
      tcp_client_write(client, buf, n);
    } else if (n == 0) {
      printf("Client #%d disconnected\n", i);
      tcp_client_dispose(client);
    }
  }
}
