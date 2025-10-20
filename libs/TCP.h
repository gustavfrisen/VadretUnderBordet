#ifndef TCP_H
#define TCP_H

#include <stddef.h>
#include <sys/types.h>

int tcp_listen(int port, int backlog);
int tcp_accept(int listen_fd);
int tcp_connect(const char *host, int port);
ssize_t tcp_read_all(int fd, char *buf, size_t maxlen);
ssize_t tcp_write_all(int fd, const char *buf, size_t len);

#endif