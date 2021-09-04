#include "cosmic/cosmic_io_mem.h"

struct cosmic_io_vtable {
  ssize_t (*read)(cosmic_io_t *, char *, size_t);
  ssize_t (*write)(cosmic_io_t *, const char *, size_t);
  ssize_t (*close)(cosmic_io_t *);
};

struct cosmic_io {
  const struct cosmic_io_vtable *vtbl;
  char *buf;
  size_t len, r, w;
};

ssize_t cosmic_io_mem_read(cosmic_io_t *io, char *buf, size_t len) {
  /**
   *  Number of bytes I can read from io->buf
   * len - i for nbytes
   */
  size_t nbytes = io->len - io->r;

  /**
   * No bytes to read
   */
  if (nbytes == 0) {
    return -1;
  }

  if (nbytes > len) {
    nbytes = len;
  }

  memcpy(buf, io->buf + io->r, nbytes);
  io->r += nbytes;
  return nbytes;
}

ssize_t cosmic_io_mem_write(cosmic_io_t *io, const char *buf, size_t len) {
  size_t nbytes = io->len - io->w;

  /**
   * Cannot write (no space)
   */
  if (nbytes == 0) {
    return -1;
  }

  if (nbytes > len) {
    nbytes = len;
  }

  memcpy(io->buf + io->w, buf, nbytes);
  io->w += nbytes;
  return nbytes;
}

struct cosmic_io_vtable COSMIC_IO_MEM_VTBL = {cosmic_io_mem_read,
                                              cosmic_io_mem_write, NULL};

cosmic_io_t *cosmic_io_mem_new(const char *buf, size_t len) {
  cosmic_io_t *io = malloc(sizeof(cosmic_io_t));
  io->vtbl = &COSMIC_IO_MEM_VTBL;
  if (buf) {
    io->buf = strdup(buf);
  } else {
    io->buf = calloc(len, sizeof(char));
  }
  io->len = len;
  io->r = io->w = 0;
  return io;
}

void cosmic_io_mem_free(cosmic_io_t *io) {
  free(io->buf);
  free(io);
}

ssize_t cosmic_io_mem_rsetpos(cosmic_io_t *io, size_t r) {
  if (r > io->len) {
    return -1;
  }
  return (io->r = r);
}

ssize_t cosmic_io_mem_wsetpos(cosmic_io_t *io, size_t w) {
  if ( w > io->len){
    return -1;
  }
  return (io->w = w);
}
