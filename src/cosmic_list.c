#include "cosmic/cosmic_list.h"
#include "internal/cosmic_iterator.h"
#include "internal/cosmic_list.h"

struct cosmic_list {
  const struct cosmic_list_vtable *vtbl;
};

ssize_t cosmic_list_size(const cosmic_list_t *l) {
  if (l->vtbl->size) {
    return l->vtbl->size(l);
  }

  return -1;
}

int cosmic_list_get(const cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  if (l->vtbl->get) {
    return l->vtbl->get(l, i, o);
  }

  return -1;
}

ssize_t cosmic_list_add(cosmic_list_t *l, cosmic_any_t o) {
  if (l->vtbl->add) {
    return l->vtbl->add(l, o);
  }

  return -1;
}

ssize_t cosmic_list_insert(cosmic_list_t *l, size_t i, cosmic_any_t o) {
  if (l->vtbl->insert) {
    return l->vtbl->insert(l, i, o);
  }

  return -1;
}

int cosmic_list_remove(cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  if (l->vtbl->remove) {
    return l->vtbl->remove(l, i, o);
  }

  return -1;
}

int cosmic_list_pop(cosmic_list_t *l, cosmic_any_t *o) {
  if (l->vtbl->pop) {
    return l->vtbl->pop(l, o);
  }

  return -1;
}

cosmic_iterator_t *cosmic_list_iterator(const cosmic_list_t *l) {
  if (l->vtbl->iterator) {
    return l->vtbl->iterator(l);
  }

  return NULL;
}

cosmic_any_t cosmic_list_get_t(const cosmic_list_t *l, size_t index, int *r) {
  int i;
  cosmic_any_t o;

  i = cosmic_list_get(l, index, &o);

  if (i != 0) {
    o = COSMIC_VOID;
  }

  if (r) {
    *r = i;
  }

  return o;
}
