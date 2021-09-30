#ifndef INTERNAL_COSMIC_LIST_H
#define INTERNAL_COSMIC_LIST_H

#include <cosmic/cosmic_list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cosmic_list_vtable {
  size_t (*size)(const cosmic_list_t *);
  int (*get)(const cosmic_list_t *, size_t, cosmic_any_t *);
  ssize_t (*add)(cosmic_list_t *, cosmic_any_t);
  ssize_t (*insert)(cosmic_list_t *, size_t, cosmic_any_t);
  int (*remove)(cosmic_list_t *, size_t, cosmic_any_t *);
  int (*pop)(cosmic_list_t *, cosmic_any_t *);
  cosmic_iterator_t *(*iterator)(const cosmic_list_t *);
};

#ifdef __cplusplus
}
#endif

#endif
