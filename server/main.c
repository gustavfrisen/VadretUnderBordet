#include "TCP.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* from select() man pages:
 WARNING: select() can monitor only file descriptors numbers that are less than
 FD_SETSIZE (1024) an unreasonably  low  limit for many modern applications and
 this limitation will not change.  All modern applications should instead use
 poll(2) or epoll(7), which do not suffer this limitation.
*/

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

  // start listening on port
  if (tcp_listen_start(server.fd, server.port) < 0) {
    return -1;
  }

  print_server_addresses(server.port);

  // init of READ file descriptor lists used by select()
  fd_set fd_list_read, fd_list_read_cpy;
  FD_ZERO(&fd_list_read);
  FD_ZERO(&fd_list_read_cpy);

  // init of WRITE file descriptor lists used by select()
  fd_set fd_list_write, fd_list_write_cpy;
  FD_ZERO(&fd_list_write);
  FD_ZERO(&fd_list_write_cpy);

  // init of ERROR file descriptor lists used by select()
  fd_set fd_list_error, fd_list_error_cpy;
  FD_ZERO(&fd_list_error);
  FD_ZERO(&fd_list_error_cpy);

  // adds servers listening socket as first fd in master
  FD_SET(server.fd, &fd_list_read);

  int fdmax = server.fd;

  while (1) {
    printf("\nListening...\n");

    if (fdmax >= FD_SETSIZE) {
      printf("max select() fd´s reached\n");
      break;
    }

    fd_list_read_cpy = fd_list_read;
    fd_list_write_cpy = fd_list_write;
    fd_list_error_cpy = fd_list_error;

    // Block until something is ready
    int activity = select(fdmax + 1, &fd_list_read_cpy, &fd_list_write_cpy,
                          &fd_list_error_cpy, NULL);
    if (activity == -1) {
      perror("select");
      break;
    }

    // Check all file descriptors select() reported as active
    for (int i = 0; i <= fdmax; i++) {
      // i is not a ready file descriptor in any list, jump to next iteration
      if (!FD_ISSET(i, &fd_list_read_cpy) && !FD_ISSET(i, &fd_list_write_cpy) &&
          !FD_ISSET(i, &fd_list_error_cpy))
        continue;

      // New connection on the server fd
      if (FD_ISSET(i, &fd_list_read_cpy) && i == server.fd) {
        Client *client = &server.clients[server.clients_count];
        client->fd = tcp_accept_blocking(server.fd, &client->address);
        if (client->fd < 0) {
          perror("accept");
          continue;
        }

        tcp_print_client_address(&client->address);

        // Add the new client to the  set
        FD_SET(client->fd, &fd_list_read);
        if (client->fd > fdmax)
          fdmax = client->fd;
        server.clients_count++;

      } else {
        // Data on an existing client socket (fd == i)
        Client *client = NULL;
        for (int c = 0; c < server.clients_count; c++) {
          if (server.clients[c].fd == i) {
            client = &server.clients[c];
            break;
          }
        }
        if (!client) {
          // Unknown fd; remove it defensively
          close(i);
          FD_CLR(i, &fd_list_read);
          continue;
        }

        // Read data
        ssize_t nbytes =
            recv(client->fd, client->buffer, sizeof(client->buffer) - 1, 0);
        if (nbytes <= 0) {
          if (nbytes == 0) {
            printf("Client on fd %d disconnected\n", client->fd);
          } else {
            perror("recv");
          }
          // Cleanup: close and remove from fd_set and clients array
          close(client->fd);
          FD_CLR(client->fd, &fd_list_read);

          // Compact clients array (swap with last, then decrement)
          server.clients_count--;
          if (client != &server.clients[server.clients_count]) {
            *client = server.clients[server.clients_count];
          }
          continue;
        }

        // Null-terminate for convenience if treating as text
        client->buffer[nbytes] = '\0';
        printf("Data available on socket %d (%zd bytes)\n", i, nbytes);

        // Handle request and respond (your function may read/process/send)
        tcp_handle_client(client->fd, client->buffer);

        // For HTTP/1.0-style: close after response
        close(client->fd);
        FD_CLR(client->fd, &fd_list_read);

        // Remove from clients array
        server.clients_count--;
        if (client != &server.clients[server.clients_count]) {
          *client = server.clients[server.clients_count];
        }
      }
    }
  }

  return 0;
}