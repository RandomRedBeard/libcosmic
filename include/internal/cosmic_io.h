#ifndef INTERNAL_COSMIC_IO_H
#define INTERNAL_COSMIC_IO_H

#include <cosmic/cosmic_io.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cosmic_io_vtable {
  ssize_t (*read)(cosmic_io_t *, char *, size_t);
  ssize_t (*write)(cosmic_io_t *, const char *, size_t);
  int (*close)(cosmic_io_t *);
};

#ifdef __cplusplus
}
#endif

#endif
