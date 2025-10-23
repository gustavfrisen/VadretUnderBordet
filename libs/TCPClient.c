#include "TCPClient.h"

// CLIENT
int tcp_client_init(tcp_client *self, const char *host, const char *port) {
  if (self == NULL) {
    return -1;
  }
  self->fd = -1;
  if (strlen(host) > HOST_SIZE - 1 || strlen(host) > PORT_SIZE - 1) {
    return -1;
  }

  strncpy(self->host, host, HOST_SIZE - 1);
  self->host[HOST_SIZE - 1] = '\0';

  strncpy(self->port, port, PORT_SIZE - 1);
  self->port[PORT_SIZE - 1] = '\0';
  return 0;
}

int tcp_client_connect(tcp_client *self) {
  if (self == NULL) {
    return -1;
  }
  struct addrinfo hints = {0}, *res = NULL;
  if (getaddrinfo(self->host, self->port, &hints, &res) != 0) {
    return -1;
  }

  int fd = -1;
  for (struct addrinfo *rp = res; rp; rp = rp->ai_next) {
    if (rp->ai_socktype != SOCK_STREAM) {
      continue;
    }

    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (fd < 0) {
      continue;
    }

    if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;
    }

    close(fd);
    fd = -1;
  }

  freeaddrinfo(res);
  if (fd < 0) {
    return -1;
  }
  self->fd = fd;
  return 0;
}

void tcp_client_disconnect(tcp_client *self) {
  if (self->fd >= 0) {
    close(self->fd);
  }
  self->fd = -1;
}
// tcp_client_read
int tcp_client_read(tcp_client *self, void *buf, size_t len) {
  if (self->fd < 0) {
    return -1;
  }
  return recv(self->fd, buf, len, MSG_NOSIGNAL);
}

int tcp_client_write(tcp_client *self, const void *buf, size_t len) {
  return send(self->fd, buf, len, MSG_DONTWAIT);
}

// tcp_client_dispose
void tcp_client_dispose(tcp_client *self) { tcp_client_disconnect(self); }
