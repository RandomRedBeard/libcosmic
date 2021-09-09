#include "cosmic/cosmic_io.h"
#include "internal/cosmic_io.h"

struct cosmic_io {
  const struct cosmic_io_vtable *vtbl;
};

ssize_t cosmic_io_read(cosmic_io_t *io, char *buf, size_t len) {
  if (!io->vtbl->read) {
    return -1;
  }

  return io->vtbl->read(io, buf, len);
}

ssize_t cosmic_io_write(cosmic_io_t *io, const char *buf, size_t len) {
  if (!io->vtbl->write) {
    return -1;
  }
  return io->vtbl->write(io, buf, len);
}

int cosmic_io_close(cosmic_io_t *io) {
  int (*close)(cosmic_io_t *) = io->vtbl->close;
  if (!close) {
    return -1;
  }
  return close(io);
}
