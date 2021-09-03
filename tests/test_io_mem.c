#include "test_base.h"
#include <cosmic/cosmic_io_mem.h>
#include <stdio.h>

void test_read() {
  cosmic_io_t *io = cosmic_io_mem_new("thomas jansen", 13);

  char buf[128];
  memset(buf, 0, 128);

  assert(cosmic_io_read(io, buf, 3) == 3);
  assert(strcmp(buf, "tho") == 0);

  assert(cosmic_io_read(io, buf, 6) == 6);
  assert(strcmp(buf, "mas ja") == 0);

  cosmic_io_mem_free(io);
}

void test_write() {
  cosmic_io_t *io = cosmic_io_mem_new(NULL, 128);
  assert(cosmic_io_write(io, "thomas", 6) == 6);

  char buf[128];
  int i = cosmic_io_read(io, buf, 6);
  assert(i == 6);
  *(buf + i) = 0;
  assert(strcmp(buf, "thomas") == 0);

  cosmic_io_mem_free(io);
}

void test_close() {
  cosmic_io_t *io = cosmic_io_mem_new(NULL, 128);
  // Close on io_mem is meaningless;
  assert(cosmic_io_close(io) == -1);
  cosmic_io_mem_free(io);
}

int main() {
  test_read();
  test_write();
  test_close();
  return 0;
}