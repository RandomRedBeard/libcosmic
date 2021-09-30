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

/* Forward declaration of io functions */

int cosmic_socket_rpoll(cosmic_io_t *io) {
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

int cosmic_socket_wpoll(cosmic_io_t *io) {
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

int cosmic_socket_shutdown(cosmic_io_t *io, int how) {
  return shutdown(io->fd, how);
}

ssize_t cosmic_socket_io_read(cosmic_io_t *io, char *buf, size_t len) {
  int r = cosmic_socket_rpoll(io);
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
    n = cosmic_socket_wpoll(io);
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

cosmic_io_t *cosmic_socket_new() {
  cosmic_io_t *s = calloc(1, sizeof(cosmic_io_t));
  s->fd = socket(AF_INET, SOCK_STREAM, 0);
  s->rpwait = s->wpwait = -1;
  s->vtbl = &COSMIC_IO_SOCKET_VTBL;

  return s;
}

void cosmic_socket_free(cosmic_io_t *s) {
  cosmic_socket_io_close(s);
  free(s);
}

/**
 * Connectors
 */

int cosmic_socket_connect(cosmic_io_t *s, const struct sockaddr *addr,
                          socklen_t addrlen) {
#ifdef _WIN32
  return connect(s->fd, addr, (int)addrlen);
#else
  return connect(s->fd, addr, addrlen);
#endif
}

int cosmic_socket_connect_to_host(cosmic_io_t *s, const char *node,
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
    if ((i = connect(s->fd, iter->ai_addr, (int)iter->ai_addrlen)) < 0) {
#else
    if ((i = connect(s->fd, iter->ai_addr, iter->ai_addrlen)) < 0) {
#endif
      iter = iter->ai_next;
      continue;
    }

    break;
  }

  freeaddrinfo(addr);
  return i;
}

int cosmic_socket_bind(cosmic_io_t *s, const struct sockaddr *addr,
                       socklen_t addrlen) {
#ifdef _WIN32
  return bind(s->fd, addr, (int)addrlen);
#else
  return bind(s->fd, addr, addrlen);
#endif
}

int cosmic_socket_bind_to_host(cosmic_io_t *s, const char *node,
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
    if ((i = bind(s->fd, iter->ai_addr, (int)iter->ai_addrlen)) < 0) {
#else
    if ((i = bind(s->fd, iter->ai_addr, iter->ai_addrlen)) < 0) {
#endif
      iter = iter->ai_next;
      continue;
    }

    break;
  }

  freeaddrinfo(addr);
  return i;
}

int cosmic_socket_listen(cosmic_io_t *s, int backlog) {
  return listen(s->fd, backlog);
}

cosmic_io_t *cosmic_socket_accept(cosmic_io_t *s) {
#ifdef _WIN32
  SOCKET fd = accept(s->fd, NULL, NULL);
#else
  int fd = accept(s->fd, NULL, NULL);
#endif
  cosmic_io_t *cl = NULL;
  if (fd < 0) {
    return NULL;
  }

  cl = calloc(1, sizeof(cosmic_io_t));
  cl->fd = fd;
  cl->rpwait = s->wpwait = -1;
  cl->vtbl = &COSMIC_IO_SOCKET_VTBL;

  return cl;
}

int cosmic_socket_set_nonblock(cosmic_io_t *s, int nonblock) {
#ifdef _WIN32
  u_long i = nonblock;
  return ioctlsocket(s->fd, FIONBIO, &i);
#else
  int flags = fcntl(s->fd, F_GETFL, 0);
  if (flags < 0) {
    return flags;
  }

  if (nonblock) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  return fcntl(s->fd, F_SETFL, flags);
#endif
}

void cosmic_socket_set_rpwait(cosmic_io_t *s, int rpwait) {
  s->rpwait = rpwait;
}

void cosmic_socket_set_wpwait(cosmic_io_t *s, int wpwait) {
  s->wpwait = wpwait;
}

#ifdef _WIN32
SOCKET cosmic_socket_get_fd(cosmic_io_t *s) { return s->fd; }
#else
int cosmic_socket_get_fd(cosmic_io_t *s) { return s->fd; }
#endif
