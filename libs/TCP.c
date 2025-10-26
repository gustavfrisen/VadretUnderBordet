#define _GNU_SOURCE // enables POSIX addrinfo stuff
#include "TCP.h"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER 4096
#define LISTEN_BACKLOG 1024 // max sockets in WSL2 is 4096

void print_server_addresses(int port) {
  // ifaddrs only on linux, not availible on freeRTOS
  struct ifaddrs *interface_addresses, *iterator;

  if (getifaddrs(&interface_addresses) == -1) {
    perror("getifaddrs");
    return;
  }

  printf("\nServer listening on:\n");

  for (iterator = interface_addresses; iterator != NULL;
       iterator = iterator->ifa_next) {
    if (iterator->ifa_addr == NULL)
      continue;

    int family = iterator->ifa_addr->sa_family;

    // Only prints IPv4 and IPv6 addresses
    if (family == AF_INET || family == AF_INET6) {
      char host[NI_MAXHOST];
      int gni_status =
          getnameinfo(iterator->ifa_addr,
                      (family == AF_INET) ? sizeof(struct sockaddr_in)
                                          : sizeof(struct sockaddr_in6),
                      host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

      if (gni_status == 0) {
        const char *type = (family == AF_INET) ? "IPv4" : "IPv6";
        if (family == AF_INET6) {
          printf("[%s]:%d (%s on %s)\n", host, port, type, iterator->ifa_name);
        } else {
          printf("%s:%d (%s on %s)\n", host, port, type, iterator->ifa_name);
        }
      }
    }
  }
  printf("\n");
  freeifaddrs(interface_addresses);
}

int tcp_init(int server_port) {

  struct addrinfo hints, *addresses, *iterator;
  int server_fd = -1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // Server mode

  // Convert port to string
  char port_str[6];
  snprintf(port_str, sizeof(port_str), "%d", server_port);

  // Get address info
  int status = getaddrinfo(NULL, port_str, &hints, &addresses);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return -1;
  }

  // Loop through results and bind to the first we can
  for (iterator = addresses; iterator != NULL; iterator = iterator->ai_next) {
    // Try to create socket with this address family
    server_fd = socket(iterator->ai_family, iterator->ai_socktype,
                       iterator->ai_protocol);
    if (server_fd < 0) {
      continue; // Try next address
    }

    // Reuse socket - fixes "socket already in use" when restarting
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0) {
      close(server_fd);
      continue; // Try next address
    }

    // Try to bind
    if (bind(server_fd, iterator->ai_addr, iterator->ai_addrlen) < 0) {
      close(server_fd);
      continue; // Try next address
    }

    // Success! Break out of loop
    break;
  }
  freeaddrinfo(addresses); // Free the linked list

  // Check if we successfully bound to any address
  if (iterator == NULL) {
    fprintf(stderr, "Failed to bind to any address\n");
    return -1;
  }
  return server_fd;
}

// returns the same fd on success and -1 on fail
int tcp_listen_start(int server_fd, int server_port) {
  // not reasonable to do socket_fd validation, trust the caller
  if (listen(server_fd, LISTEN_BACKLOG) < 0) {
    perror("listen failed");
    return -1;
  }
  return server_fd;
}

// This accept() call blocks. Client_address can be NULL
int tcp_accept_blocking(int server_fd,
                        struct sockaddr_storage *client_address) {
  socklen_t addrlen;
  int client_fd;

  // TODO implement timeout

  if (client_address != NULL) {
    addrlen = sizeof(*client_address);
  }

  client_fd = accept(server_fd,
                     client_address ? (struct sockaddr *)client_address : NULL,
                     client_address ? &addrlen : NULL);

  return client_fd;
}

void tcp_print_client_address(struct sockaddr_storage *client_address) {
  char ip_string[INET6_ADDRSTRLEN]; // Big enough for IPv6
  uint16_t port;
  void *address_ptr;
  const char *ip_version;

  // Check if address is NULL
  if (client_address == NULL) {
    printf("No client address available\n");
    return;
  }

  // Handle IPv4 vs IPv6
  if (client_address->ss_family == AF_INET) {
    // IPv4
    struct sockaddr_in *s = (struct sockaddr_in *)client_address;
    address_ptr = &s->sin_addr;
    port = ntohs(s->sin_port);
    ip_version = "IPv4";

  } else if (client_address->ss_family == AF_INET6) {
    // IPv6
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)client_address;
    address_ptr = &s->sin6_addr;
    port = ntohs(s->sin6_port);
    ip_version = "IPv6";

  } else {
    printf("Unknown address family: %d\n", client_address->ss_family);
    return;
  }

  // Convert binary address to string
  if (inet_ntop(client_address->ss_family, address_ptr, ip_string,
                sizeof(ip_string)) == NULL) {
    perror("inet_ntop");
    return;
  }

  // Print the result
  printf("Client connected from %s: %s:%d\n", ip_version, ip_string, port);
}

// test in terminal with "echo "there is an echo here" | nc localhost 6543"
int tcp_echo(int client_fd, char *buffer, size_t buffer_size) {
  // Receive data
  ssize_t n = recv(client_fd, buffer, buffer_size, 0);

  if (n < 0) {
    perror("recv");
    return -1;
  }

  if (n == 0) {
    // Client closed connection
    return 0;
  }

  ssize_t total_sent = 0;

  while (total_sent < n) {
    ssize_t sent = send(client_fd, buffer + total_sent, n - total_sent, 0);
    if (sent < 0) {
      perror("send");
      return -1;
    }
    total_sent += sent;
  }

  return n;
}

void tcp_handle_client(int client_fd, char *buffer) {
  recv(client_fd, buffer, sizeof(buffer), 0);

  const char *json_response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: 38\r\n"
                              "\r\n"
                              "{\"status\":\"ok\",\"message\":\"Hello World\"}";

  send(client_fd, json_response, strlen(json_response), 0);
}

#include <string.h>
#include <unistd.h>

int tcp_deny_connection(int client_fd) {
  ssize_t send_status = send(client_fd, HTTP_503_SERVICE_UNAVAILABLE,
                             strlen(HTTP_503_SERVICE_UNAVAILABLE), 0);
  if (send_status < 0) {
    perror("send (deny)");
  }
  return 0;
}
