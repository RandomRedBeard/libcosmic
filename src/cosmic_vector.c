#include "cosmic/cosmic_vector.h"
#include "internal/cosmic_iterator.h"
#include "internal/cosmic_list.h"

#include <stdio.h>

struct cosmic_list {
  const struct cosmic_list_vtable *vtbl;
  size_t len, alloc_size;
  cosmic_any_t *block;
};

struct cosmic_iterator {
  const struct cosmic_iterator_vtable *vtbl;
  cosmic_any_t *block, *end;
};

size_t cosmic_vector_size(const cosmic_list_t *l) { return l->len; }

int cosmic_vector_get(const cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  if (i >= l->len) {
    return -1;
  }

  *o = *(l->block + i);
  return 0;
}

ssize_t cosmic_vector_add(cosmic_list_t *l, cosmic_any_t o) {
  /* Realloc to the block */
  if (l->len != 0 && l->len % l->alloc_size == 0) {
    l->block =
        realloc(l->block, sizeof(cosmic_any_t) * (l->len + l->alloc_size));
  }

  *(l->block + l->len++) = o;
  return l->len;
}

ssize_t cosmic_vector_insert(cosmic_list_t *l, size_t i, cosmic_any_t o) {
  cosmic_any_t buf;
  if (i > l->len) {
    return -1;
  }

  l->len++;

  /* Realloc to the block */
  if (l->len % l->alloc_size == 0) {
    l->block =
        realloc(l->block, sizeof(cosmic_any_t) * (l->len + l->alloc_size));
  }

  while (i < l->len) {
    buf = *(l->block + i);
    *(l->block + i) = o;
    o = buf;
    i++;
  }

  return 0;
}

int cosmic_vector_remove(cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  if (i >= l->len) {
    return -1;
  }

  *o = *(l->block + i++);
  while (i < l->len) {
    *(l->block + i - 1) = *(l->block + i);
    i++;
  }

  l->len--;

  if (l->len % l->alloc_size == 0) {
    l->block =
        realloc(l->block, sizeof(cosmic_any_t) * (l->len + l->alloc_size));
  }

  return 0;
}

int cosmic_vector_pop(cosmic_list_t *l, cosmic_any_t *o) {
  return cosmic_vector_remove(l, 0, o);
}

int cosmic_vector_iterator_next(cosmic_iterator_t *it, cosmic_any_t *o) {
  if (it->block == it->end) {
    return -1;
  }

  *o = *it->block;
  it->block++;
  return 0;
}

int cosmic_vecotr_iterator_has_next(const cosmic_iterator_t *it) {
  return it->block != it->end;
}

void cosmic_vecotr_iterator_close(cosmic_iterator_t *it) { free(it); }

struct cosmic_iterator_vtable COSMIC_VECTOR_ITERATOR_VTBL = {
    cosmic_vector_iterator_next, NULL, cosmic_vecotr_iterator_has_next,
    cosmic_vecotr_iterator_close};

cosmic_iterator_t *cosmic_vector_iterator(const cosmic_list_t *l) {
  cosmic_iterator_t *it = malloc(sizeof(struct cosmic_iterator));
  it->vtbl = &COSMIC_VECTOR_ITERATOR_VTBL;
  it->block = l->block;
  it->end = l->block + l->len;
  return it;
}

struct cosmic_list_vtable COSMIC_VECTOR_VTBL = {
    cosmic_vector_size,    cosmic_vector_get,    cosmic_vector_add,
    cosmic_vector_insert,  cosmic_vector_remove, cosmic_vector_pop,
    cosmic_vector_iterator};

cosmic_list_t *cosmic_vector_new(size_t alloc_size) {
  cosmic_list_t *l = malloc(sizeof(cosmic_list_t));
  l->vtbl = &COSMIC_VECTOR_VTBL;
  l->alloc_size = alloc_size;
  l->len = 0;
  l->block = malloc(sizeof(cosmic_any_t) * l->alloc_size);
  return l;
}

void cosmic_vector_free(cosmic_list_t *l, cosmic_list_dealloc dealloc) {
  size_t i;
  if (dealloc) {
    for (i = 0; i < l->len; i++) {
      dealloc(*(l->block + i));
    }
  }
  free(l->block);
  free(l);
}
