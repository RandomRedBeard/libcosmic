#include "test_base.h"
#include <cosmic/cosmic_socket.h>
#include <errno.h>
#include <stdio.h>

void test_connect() {
  const char *get = "GET /\r\n\r\n";
  char buf[1024];
  ssize_t i;
  cosmic_socket_t *s = cosmic_socket_new("google.com", "http");
  cosmic_socket_set_nonblock(s, 1);
  if (cosmic_socket_connect(s) < 0) {
    perror("Initial connect failed");
    if (errno != EINPROGRESS) {
      perror("Failed to connect");
      exit(1);
    }
    if (cosmic_socket_wpoll(s) < 0) {
      perror("Failed to connect");
      exit(1);
    }
  }

  /* Set poll wait for read */
  cosmic_socket_set_rpwait(s, 1000);

  assert(cosmic_socket_write(s, get, strlen(get)) > 0);

  while ((i = cosmic_socket_read(s, buf, 1023)) > 0) {
    *(buf + i) = 0;
    printf("%ld\n", i);
  }

  cosmic_socket_free(s);
}

int main() {
  cosmic_socket_init();
  test_connect();
  return 0;
}
