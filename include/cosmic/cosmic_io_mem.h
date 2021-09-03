#ifndef COSMIC_IO_MEM_H
#define COSMIC_IO_MEM_H

#include <stdlib.h>
#include <string.h>

#include <cosmic/cosmic_io.h>
#include <cosmic/cosmic_lib.h>

#ifdef __cplusplus
extern "C" {
#endif

COSMIC_DLL cosmic_io_t *cosmic_io_mem_new(const char *, size_t);
COSMIC_DLL void cosmic_io_mem_free(cosmic_io_t *);

#ifdef __cplusplus
}
#endif

#endif