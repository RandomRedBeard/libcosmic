#ifndef INTERNAL_COSMIC_DMAP_H
#define INTERNAL_COSMIC_DMAP_H

#include <cosmic/cosmic_map.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cosmic_map_vtable {
  size_t (*size)(const cosmic_map_t *);
  int (*get_value)(const cosmic_map_t *, const cosmic_any_t, cosmic_any_t *);
  ssize_t (*add)(cosmic_map_t *, cosmic_any_t, cosmic_any_t);
  int (*remove)(cosmic_map_t *, const cosmic_any_t, cosmic_pair_t *);
  cosmic_iterator_t *(*iterator)(const cosmic_map_t *);
};

#ifdef __cplusplus
}
#endif

#endif
