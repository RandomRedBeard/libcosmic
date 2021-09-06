#include "cosmic/cosmic_socket.h"

int cosmic_socket_init() {
#if defined(_WIN32) || (_WIN64)
  WSADATA wsa;
  return WSAStartup(MAKEWORD(2, 2), &wsa);
#else
  return 0;
#endif
}

struct cosmic_io_vtable {
  ssize_t (*read)(cosmic_io_t *, char *, size_t);
  ssize_t (*write)(cosmic_io_t *, const char *, size_t);
  int (*close)(cosmic_io_t *);
};

/* Underlying socket io */
struct cosmic_io {
  const struct cosmic_io_vtable *vtbl;
  int fd, rpwait, wpwait;
};

struct cosmic_socket {
  cosmic_io_t io;
  struct addrinfo *addr;
};

/* Forward declaration of io functions */

int cosmic_socket_io_rpoll(cosmic_io_t *io) {
  struct pollfd pfd;
  int n, pollerr = POLLNVAL | POLLERR;

  pfd.fd = io->fd;
  pfd.events = POLLIN;

  n = poll(&pfd, 1, io->rpwait);

  if (n < 0) {
    return -1;
  }

  if (pfd.revents & pollerr) {
    return -1;
  }

  /* Recieved hangup and no bytes to read */
  if (pfd.revents & POLLHUP && !(pfd.revents & POLLIN)) {
    return -1;
  }

  return n;
}

int cosmic_socket_io_wpoll(cosmic_io_t *io) {
  struct pollfd pfd;
  int n, pollerr = POLLHUP | POLLNVAL | POLLERR;

  pfd.fd = io->fd;
  pfd.events = POLLOUT;

  n = poll(&pfd, 1, io->wpwait);

  if (n < 0) {
    return -1;
  }

  if (pfd.revents & pollerr) {
    return -1;
  }

  return n;
}

int cosmic_socket_io_shutdown(cosmic_io_t *io, int how) {
  return shutdown(io->fd, how);
}

ssize_t cosmic_socket_io_read(cosmic_io_t *io, char *buf, size_t len) {
  int r = cosmic_socket_io_rpoll(io);
  if (r <= 0) {
    return r;
  }

  return recv(io->fd, buf, len, 0);
}

ssize_t cosmic_socket_io_write(cosmic_io_t *io, const char *buf, size_t len) {
  ssize_t n;
  size_t nbytes = 0;
  while (nbytes < len) {
    n = cosmic_socket_io_wpoll(io);
    if (n <= 0) {
      return n;
    }

    n = send(io->fd, buf + nbytes, len - nbytes, 0);
    if (n < 0) {
      return n;
    }

    nbytes += n;
  }

  return nbytes;
}

int cosmic_socket_io_close(cosmic_io_t *io) {
#ifdef _WIN32
  return closesocket(io->fd);
#else
  return close(io->fd);
#endif
}

struct cosmic_io_vtable COSMIC_IO_SOCKET_VTBL = {
    cosmic_socket_io_read, cosmic_socket_io_write, cosmic_socket_io_close};

cosmic_socket_t *cosmic_socket_new(const char *node, const char *service) {
  cosmic_socket_t *s = NULL;
  struct addrinfo hints, *addr;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(node, service, &hints, &addr) != 0) {
    return NULL;
  }

  s = calloc(1, sizeof(cosmic_socket_t));
  s->addr = addr;
  s->io.fd = socket(AF_INET, SOCK_STREAM, 0);
  s->io.rpwait = s->io.wpwait = -1;

  return s;
}

void cosmic_socket_free(cosmic_socket_t *s) {
  cosmic_socket_close(s);
  freeaddrinfo(s->addr);
  free(s);
}

/**
 * Connectors
 */

int cosmic_socket_connect(cosmic_socket_t *s) {
  return connect(s->io.fd, s->addr->ai_addr, s->addr->ai_addrlen);
}

int cosmic_socket_bind(cosmic_socket_t *s) {
  return bind(s->io.fd, s->addr->ai_addr, s->addr->ai_addrlen);
}

int cosmic_socket_listen(cosmic_socket_t *s, int backlog) {
  return listen(s->io.fd, backlog);
}

/**
 * IO Function socket wrappers
 */

ssize_t cosmic_socket_read(cosmic_socket_t *s, char *buf, size_t len) {
  return cosmic_socket_io_read(&s->io, buf, len);
}

ssize_t cosmic_socket_write(cosmic_socket_t *s, const char *buf, size_t len) {
  return cosmic_socket_io_write(&s->io, buf, len);
}

int cosmic_socket_close(cosmic_socket_t *s) {
  return cosmic_socket_io_close(&s->io);
}

/**
 * Socket specific functions
 */

int cosmic_socket_rpoll(cosmic_socket_t *s) {
  return cosmic_socket_io_rpoll(&s->io);
}

int cosmic_socket_wpoll(cosmic_socket_t *s) {
  return cosmic_socket_io_wpoll(&s->io);
}

int cosmic_socket_shutdown(cosmic_socket_t *s, int how) {
  return cosmic_socket_io_shutdown(&s->io, how);
}

void cosmic_socket_set_rpwait(cosmic_socket_t *s, int rpwait) {
  s->io.rpwait = rpwait;
}

void cosmic_socket_set_wpwait(cosmic_socket_t *s, int wpwait) {
  s->io.wpwait = wpwait;
}

cosmic_io_t *cosmic_socket_get_io(cosmic_socket_t *s) { return &s->io; }

int cosmic_socket_get_fd(cosmic_socket_t *s) { return s->io.fd; }
