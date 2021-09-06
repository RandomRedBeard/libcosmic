#ifndef COSMIC_IO_H
#define COSMIC_IO_H

#include <cosmic/cosmic_lib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cosmic_io cosmic_io_t;

COSMIC_DLL ssize_t cosmic_io_read(cosmic_io_t *, char *, size_t);
COSMIC_DLL ssize_t cosmic_io_write(cosmic_io_t *, const char *, size_t);
COSMIC_DLL int cosmic_io_close(cosmic_io_t *);
COSMIC_DLL ssize_t cosmic_io_copy(cosmic_io_t *, cosmic_io_t *, size_t);

#ifdef USING_NAMESPACE_COSMIC

typedef cosmic_io_t io_t;
#define io_read cosmic_io_read
#define io_write cosmic_io_write
#define io_close cosmic_io_close
#define io_copy cosmic_io_copy

#endif

#ifdef __cplusplus
}
#endif

#endif
