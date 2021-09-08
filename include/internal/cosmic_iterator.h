#ifndef INTERNAL_COSMIC_ITERATOR_H
#define INTERNAL_COSMIC_ITERATOR_H

#include <cosmic/cosmic_iterator.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cosmic_iterator_vtable {
  int (*next)(cosmic_iterator_t *, cosmic_any_t *);
  int (*next_pair)(cosmic_iterator_t *, cosmic_pair_t *);
  int (*has_next)(const cosmic_iterator_t *);
  void (*close)(cosmic_iterator_t *);
};

#ifdef __cplusplus
}
#endif

#endif