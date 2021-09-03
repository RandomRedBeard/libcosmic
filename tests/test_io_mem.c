#include <stdio.h>

#include <cosmic/cosmic_io_mem.h>

int main() {
  cosmic_io_t *io = cosmic_io_mem_new("thomas", 6);

  char buf[128];
  memset(buf, 0, 128);

  cosmic_io_read(io, buf, 3);
  puts(buf);

  cosmic_io_mem_free(io);
  return 0;
}