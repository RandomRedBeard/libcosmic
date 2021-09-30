#include "cosmic/cosmic_map.h"
#include "internal/cosmic_iterator.h"
#include "internal/cosmic_map.h"

struct cosmic_map {
  const struct cosmic_map_vtable *vtbl;
};

ssize_t cosmic_map_size(const cosmic_map_t *m) {
  if (m->vtbl->size) {
    return m->vtbl->size(m);
  }

  return -1;
}

int cosmic_map_get_value(const cosmic_map_t *m, const cosmic_any_t k,
                         cosmic_any_t *v) {
  if (m->vtbl->get_value) {
    return m->vtbl->get_value(m, k, v);
  }

  return -1;
}

ssize_t cosmic_map_add(cosmic_map_t *m, cosmic_any_t k, cosmic_any_t v) {
  if (m->vtbl->add) {
    return m->vtbl->add(m, k, v);
  }

  return -1;
}

int cosmic_map_remove(cosmic_map_t *m, const cosmic_any_t k, cosmic_pair_t *p) {
  if (m->vtbl->remove) {
    return m->vtbl->remove(m, k, p);
  }

  return -1;
}

cosmic_iterator_t *cosmic_map_iterator(const cosmic_map_t *m) {
  if (m->vtbl->iterator) {
    return m->vtbl->iterator(m);
  }

  return NULL;
}
