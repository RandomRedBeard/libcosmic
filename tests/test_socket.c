#include "test_base.h"
#include <cosmic/cosmic_socket.h>
#include <errno.h>
#include <stdio.h>

void test_connect() {
  const char *get = "GET /\r\n\r\n";
  char buf[1024];
  ssize_t i;
  cosmic_socket_t *s = cosmic_socket_new();
  cosmic_socket_set_nonblock(s, 1);
  if (cosmic_socket_connect_to_host(s, "google.com", "http") < 0) {
#ifdef _WIN32
    if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
    if (errno != EINPROGRESS) {
#endif
      perror("Failed to connect");
      exit(1);
    }
    if (cosmic_socket_wpoll(s) < 0) {
      perror("Failed to poll");
      exit(1);
    }
  }

  /* Set poll wait for read */
  cosmic_socket_set_rpwait(s, 1000);

  assert(cosmic_socket_write(s, get, strlen(get)) > 0);

  while ((i = cosmic_socket_read(s, buf, 1023)) > 0) {
    *(buf + i) = 0;
    printf("%ld\n", (long)i);
  }

  cosmic_socket_free(s);
}

void test_socket_pair() {
  cosmic_socket_t *master = cosmic_socket_new();
  cosmic_socket_t *server = NULL, *client = NULL;
  cosmic_io_t *sio, *cio;
  ssize_t i;

  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);

  char buf[1024];

  if (cosmic_socket_bind_to_host(master, "localhost", NULL) < 0) {
    perror("Failed to bind to address");
    exit(1);
  }

  if (cosmic_socket_listen(master, 0) < 0) {
    perror("Failed to listen to socket");
    exit(1);
  }

  if (getsockname(cosmic_socket_get_fd(master), (struct sockaddr *)&addr,
                  &addrlen) < 0) {
    perror("Failed to get sock name");
    exit(1);
  }

  client = cosmic_socket_new();
  if (cosmic_socket_connect(client, (struct sockaddr *)&addr, addrlen) < 0) {
    perror("Failed to connect");
#ifdef _WIN32
    printf("WSA Error %d\n", WSAGetLastError());
#endif
    exit(1);
  }

  server = cosmic_socket_accept(master);
  if (!server) {
    perror("Failed to accept");
    exit(1);
  }

  /* Close master */
  i = cosmic_socket_close(master);
  printf("Close %ld\n", (long)i);

  sio = cosmic_socket_get_io(server);
  cio = cosmic_socket_get_io(client);

  i = cosmic_io_write(sio, "test", 4);
  assert(i > 0);
  printf("Wrote %ld\n", (long)i);

  i = cosmic_io_read(cio, buf, 1023);
  assert(i > 0);
  *(buf + i) = 0;
  printf("Recieved %ld - %s\n", (long)i, buf);

  assert(strcmp(buf, "test") == 0);

  cosmic_socket_free(client);
  cosmic_socket_free(server);
  cosmic_socket_free(master);
}

void test_bind() {
  cosmic_socket_t *m1 = cosmic_socket_new(), *m2 = cosmic_socket_new();

  if (cosmic_socket_bind_to_host(m1, "localhost", "8080") < 0) {
    perror("Bind failed");
    exit(1);
  }

  assert(cosmic_socket_bind_to_host(m2, "localhost", "8080") < 0);
  perror("Failed to bind");
#ifdef _WIN32
  printf("WSA Error %d\n", WSAGetLastError());
#endif

  cosmic_socket_close(m1);

  assert(cosmic_socket_bind_to_host(m2, "localhost", "8080") >= 0);

  cosmic_socket_free(m1);
  cosmic_socket_free(m2);
}

int main() {
  cosmic_socket_init();
  test_connect();
  test_socket_pair();
  test_bind();
  return 0;
}
