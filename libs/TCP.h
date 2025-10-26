#ifndef TCP_H
#define TCP_H

#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_CLIENTS 3

typedef struct {
  int fd;
  struct sockaddr_storage address;
  char ip[INET6_ADDRSTRLEN];
  char buffer[4096];
} Client;

typedef struct {
  int fd;
  int port;
  int clients_count;
  Client clients[MAX_CLIENTS];
} Server;

int tcp_init(int server_port);
int tcp_listen_start(int server_fd, int server_port);
int tcp_deny_connection(int server_fd);
int tcp_accept_blocking(int server_fd, struct sockaddr_storage *client_address);
void tcp_print_client_address(struct sockaddr_storage *client_address);
int tcp_echo(int client_fd, char *buffer, size_t buffer_size);
void tcp_handle_client(int client_fd, char *buffer);

void print_server_addresses(int port);

#endif // TCP_H
