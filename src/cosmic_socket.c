#include "cosmic/cosmic_socket.h"
#include "internal/cosmic_io.h"

int cosmic_socket_init() {
#if defined(_WIN32) || (_WIN64)
  WSADATA wsa;
  return WSAStartup(MAKEWORD(2, 2), &wsa);
#else
  return 0;
#endif
}

/* Underlying socket io */
struct cosmic_io {
  const struct cosmic_io_vtable *vtbl;
#ifdef _WIN32
  SOCKET fd;
#else
  int fd;
#endif
  int rpwait, wpwait;
};

struct cosmic_socket {
  cosmic_io_t io;
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

#ifdef _WIN32
  return recv(io->fd, buf, (int)len, 0);
#else
  return recv(io->fd, buf, len, 0);
#endif
}

ssize_t cosmic_socket_io_write(cosmic_io_t *io, const char *buf, size_t len) {
#ifdef _WIN32
  int n;
#else
  ssize_t n;
#endif
  size_t nbytes = 0;
  while (nbytes < len) {
    n = cosmic_socket_io_wpoll(io);
    if (n <= 0) {
      return n;
    }

#ifdef _WIN32
    n = send(io->fd, buf + nbytes, (int)(len - nbytes), 0);
#else
    n = send(io->fd, buf + nbytes, len - nbytes, 0);
#endif
    if (n < 0) {
      return n;
    }

    nbytes += n;
  }

  return nbytes;
}

int cosmic_socket_io_close(cosmic_io_t *io) {
#ifdef _WIN32
  int r = -1;
  if (io->fd == INVALID_SOCKET) {
    return -1;
  }
  r = closesocket(io->fd);
  io->fd = INVALID_SOCKET;
  return r;
#else
  int r = -1;
  if (io->fd < 0) {
    return -1;
  }

  r = close(io->fd);
  io->fd = -1;
  return r;
#endif
}

struct cosmic_io_vtable COSMIC_IO_SOCKET_VTBL = {
    cosmic_socket_io_read, cosmic_socket_io_write, cosmic_socket_io_close};

cosmic_socket_t *cosmic_socket_new() {
  cosmic_socket_t *s = calloc(1, sizeof(cosmic_socket_t));
  s->io.fd = socket(AF_INET, SOCK_STREAM, 0);
  s->io.rpwait = s->io.wpwait = -1;
  s->io.vtbl = &COSMIC_IO_SOCKET_VTBL;

  return s;
}

void cosmic_socket_free(cosmic_socket_t *s) {
  cosmic_socket_close(s);
  free(s);
}

/**
 * Connectors
 */

int cosmic_socket_connect(cosmic_socket_t *s, const struct sockaddr *addr,
                          socklen_t addrlen) {
#ifdef _WIN32
  return connect(s->io.fd, addr, (int)addrlen);
#else
  return connect(s->io.fd, addr, addrlen);
#endif
}

int cosmic_socket_connect_to_host(cosmic_socket_t *s, const char *node,
                                  const char *service) {
  struct addrinfo hints, *addr = NULL, *iter = NULL;
  int i = 0;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(node, service, &hints, &addr) != 0) {
    return -1;
  }

  iter = addr;

  while (iter) {
#ifdef _WIN32
    if ((i = connect(s->io.fd, iter->ai_addr, (int)iter->ai_addrlen)) < 0) {
#else
    if ((i = connect(s->io.fd, iter->ai_addr, iter->ai_addrlen)) < 0) {
#endif
      iter = iter->ai_next;
      continue;
    }

    break;
  }

  freeaddrinfo(addr);
  return i;
}

int cosmic_socket_bind(cosmic_socket_t *s, const struct sockaddr *addr,
                       socklen_t addrlen) {
#ifdef _WIN32
  return bind(s->io.fd, addr, (int)addrlen);
#else
  return bind(s->io.fd, addr, addrlen);
#endif
}

int cosmic_socket_bind_to_host(cosmic_socket_t *s, const char *node,
                               const char *service) {
  struct addrinfo hints, *addr = NULL, *iter = NULL;
  int i = 0;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(node, service, &hints, &addr) != 0) {
    return -1;
  }

  iter = addr;

  while (iter) {
#ifdef _WIN32
    if ((i = bind(s->io.fd, iter->ai_addr, (int)iter->ai_addrlen)) < 0) {
#else
    if ((i = bind(s->io.fd, iter->ai_addr, iter->ai_addrlen)) < 0) {
#endif
      iter = iter->ai_next;
      continue;
    }

    break;
  }

  freeaddrinfo(addr);
  return i;
}

int cosmic_socket_listen(cosmic_socket_t *s, int backlog) {
  return listen(s->io.fd, backlog);
}

cosmic_socket_t *cosmic_socket_accept(cosmic_socket_t *s) {
#ifdef _WIN32
  SOCKET fd = accept(s->io.fd, NULL, NULL);
#else
  int fd = accept(s->io.fd, NULL, NULL);
#endif
  if (fd < 0) {
    return NULL;
  }

  cosmic_socket_t *cl = calloc(1, sizeof(cosmic_socket_t));
  cl->io.fd = fd;
  cl->io.rpwait = s->io.wpwait = -1;
  cl->io.vtbl = &COSMIC_IO_SOCKET_VTBL;

  return cl;
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

int cosmic_socket_set_nonblock(cosmic_socket_t *s, int nonblock) {
#ifdef _WIN32
  u_long i = nonblock;
  return ioctlsocket(s->io.fd, FIONBIO, &i);
#else
  int flags = fcntl(s->io.fd, F_GETFL, 0);
  if (flags < 0) {
    return flags;
  }

  if (nonblock) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  return fcntl(s->io.fd, F_SETFL, flags);
#endif
}

void cosmic_socket_set_rpwait(cosmic_socket_t *s, int rpwait) {
  s->io.rpwait = rpwait;
}

void cosmic_socket_set_wpwait(cosmic_socket_t *s, int wpwait) {
  s->io.wpwait = wpwait;
}

cosmic_io_t *cosmic_socket_get_io(cosmic_socket_t *s) { return &s->io; }

#ifdef _WIN32
SOCKET cosmic_socket_get_fd(cosmic_socket_t *s) { return s->io.fd; }
#else
int cosmic_socket_get_fd(cosmic_socket_t *s) { return s->io.fd; }
#endif
