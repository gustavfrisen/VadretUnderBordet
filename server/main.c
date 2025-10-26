#include "TCP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 6543

int main(int argc, char *argv[]) {
  // TODO handle command line arguments here

  // TODO move this stuff to server init function
  Server server = {0};
  server.port = SERVER_PORT;
  server.clients_count = 0;
  memset(server.clients, 0, sizeof(server.clients));
  server.fd = tcp_init(server.port);
  if (server.fd < 0) {
    return -1;
  }

  if (tcp_listen_start(server.fd, server.port) < 0) {
    return -1;
  }

  print_server_addresses(SERVER_PORT);

  while (1) {
    printf("\nWaiting for connection...\n");

    // holds current client spot in server list
    Client *client = &server.clients[server.clients_count];

    client->fd = tcp_accept_blocking(server.fd, &client->address);

    if (client->fd < 0) {
      perror("accept");
      continue;
    }

    server.clients_count++;

    tcp_print_client_address(&client->address);

    // Handle client request
    tcp_handle_client(client->fd, client->buffer);

    close(client->fd);
    server.clients_count--;
  }

  return 0;
}