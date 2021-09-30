#include "cosmic/cosmic_io_fs.h"
#include "internal/cosmic_io.h"

struct cosmic_io {
  const struct cosmic_io_vtable *vtbl;
  int fd;
};

ssize_t cosmic_io_fs_read(cosmic_io_t *io, char *buf, size_t len) {
  return read(io->fd, buf, len);
}

ssize_t cosmic_io_fs_write(cosmic_io_t *io, const char *buf, size_t len) {
  return write(io->fd, buf, len);
}

int cosmic_io_fs_close(cosmic_io_t *io) { return close(io->fd); }

struct cosmic_io_vtable COSMIC_IO_FS_VTBL = {
    cosmic_io_fs_read, cosmic_io_fs_write, cosmic_io_fs_close};

cosmic_io_t *cosmic_io_fs_new(int fd) {
  cosmic_io_t *io = malloc(sizeof(cosmic_io_t));
  io->fd = fd;
  io->vtbl = &COSMIC_IO_FS_VTBL;
  return io;
}

void cosmic_io_fs_free(cosmic_io_t *io) { free(io); }
