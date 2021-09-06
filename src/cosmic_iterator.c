#include "cosmic/cosmic_iterator.h"

struct cosmic_iterator_vtable {
  int (*next)(cosmic_iterator_t *, cosmic_any_t *);
  int (*next_pair)(cosmic_iterator_t *, cosmic_pair_t *);
  int (*has_next)(const cosmic_iterator_t *);
  void (*close)(cosmic_iterator_t *);
};

struct cosmic_iterator {
  const struct cosmic_iterator_vtable *vtbl;
};

int cosmic_iterator_next(cosmic_iterator_t *it, cosmic_any_t *o) {
  if (!it->vtbl->next) {
    return -1;
  }
  return it->vtbl->next(it, o);
}

int cosmic_iterator_next_pair(cosmic_iterator_t *it, cosmic_pair_t *p) {
  if (!it->vtbl->next_pair) {
    return -1;
  }
  return it->vtbl->next_pair(it, p);
}

int cosmic_iterator_has_next(const cosmic_iterator_t *it) {
  if (!it->vtbl->has_next) {
    return 0;
  }
  return it->vtbl->has_next(it);
}

void cosmic_iterator_close(cosmic_iterator_t *it) {
  void (*close)(cosmic_iterator_t *) = it->vtbl->close;
  if (close) {
    close(it);
  }
}
