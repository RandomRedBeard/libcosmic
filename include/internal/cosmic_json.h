#ifndef INTERNAL_COSMIC_JSON_H
#define INTERNAL_COSMIC_JSON_H

#include <cosmic/cosmic_json.h>

#ifdef __cplusplus
extern "C" {
#endif

void cosmic_json_error_ctor(cosmic_json_error_t *,
                            enum cosmic_json_error, unsigned long,
                            const char *);
/* Error build is only for internals */
cosmic_json_t *cosmic_json_error_new(const cosmic_json_error_t);

struct cosmic_json_rw_st {
  cosmic_io_t *io;
  unsigned int depth; /* Function depth */
  cosmic_json_error_t *error;
};

#ifdef __cplusplus
}
ß
#endif

#endif
