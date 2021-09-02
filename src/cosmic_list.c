#include "cosmic/cosmic_list.h"

struct cosmic_list_node {
  size_t i;
  cosmic_any_t o;
  struct cosmic_list_node *next;
};

struct cosmic_list {
  size_t len;
  struct cosmic_list_node *head;
};

struct cosmic_iterator_vtable {
  int (*next)(cosmic_iterator_t *, cosmic_any_t *);
  int (*next_pair)(cosmic_iterator_t *, cosmic_pair_t *);
  void (*close)(cosmic_iterator_t *);
};

struct cosmic_iterator {
  const struct cosmic_iterator_vtable *vtbl;
  struct cosmic_list_node *node;
};

struct cosmic_list_node *cosmic_list_node_new(size_t i, cosmic_any_t o) {
  struct cosmic_list_node *n =
      (struct cosmic_list_node *)malloc(sizeof(struct cosmic_list_node));
  n->i = i;
  n->o = o;
  n->next = NULL;
  return n;
}

void cosmic_list_node_free(struct cosmic_list_node *n) { free(n); }

cosmic_list_t *cosmic_list_new() {
  return (cosmic_list_t *)calloc(1, sizeof(cosmic_list_t));
}

void cosmic_list_free(cosmic_list_t *l, cosmic_list_dealloc dealloc) {
  cosmic_any_t o;
  while (cosmic_list_pop(l, &o) >= 0) {
    if (dealloc) {
      dealloc(o);
    }
  }
  free(l);
}

size_t cosmic_list_size(const cosmic_list_t *l) { return l->len; }

struct cosmic_list_node *cosmic_list_get_node(const cosmic_list_t *l,
                                              size_t i) {
  if (l->len == 0 || !l->head) {
    return NULL;
  }

  if (i < 0 || i >= l->len) {
    return NULL;
  }

  struct cosmic_list_node *n = l->head;
  while (n && n->i != i) {
    n = n->next;
  }

  return n;
}

int cosmic_list_get(const cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  struct cosmic_list_node *n = cosmic_list_get_node(l, i);
  if (!n) {
    return -1;
  }

  *o = n->o;
  return 0;
}

int cosmic_list_add(cosmic_list_t *l, cosmic_any_t o) {
  struct cosmic_list_node *n = cosmic_list_node_new(l->len++, o);

  if (!l->head) {
    l->head = n;
    return l->len;
  }

  n->next = l->head;
  l->head = n;

  return l->len;
}

int cosmic_list_insert(cosmic_list_t *l, size_t i, cosmic_any_t o) {
  if (i > l->len) {
    return -1;
  }

  // Insert at o on empty list
  if (i == 0 && !l->head) {
    return cosmic_list_add(l, o);
  }

  struct cosmic_list_node *n = cosmic_list_node_new(i, o);
  struct cosmic_list_node *prev =
      i == 0 ? NULL : cosmic_list_get_node(l, i - 1);

  if (i == 0) {
    n->next = l->head;
    l->head = n;
  } else {
    n->next = prev->next;
    prev->next = n;
  }

  n = n->next;
  while (n) {
    n->i++;
    n = n->next;
  }

  return ++l->len;
}

int cosmic_list_remove(cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  if (l->len == 0 || !l->head) {
    return -1;
  }

  if (i < 0 || i >= l->len) {
    return -1;
  }

  // Head case is different than other
  struct cosmic_list_node *n = NULL;
  if (i == 0) {
    n = l->head;
    l->head = l->head->next;
  } else {
    struct cosmic_list_node *prev = cosmic_list_get_node(l, i - 1);
    n = prev->next;
    prev->next = prev->next->next;
  }

  // Decrement following nodes
  struct cosmic_list_node *inc = n;
  while (inc) {
    inc->i--;
    inc = inc->next;
  }

  // Decrement len
  l->len--;

  *o = n->o;

  cosmic_list_node_free(n);

  return 0;
}

int cosmic_list_pop(cosmic_list_t *l, cosmic_any_t *o) {
  return cosmic_list_remove(l, 0, o);
}

int cosmic_list_iterator_next(cosmic_iterator_t *it, cosmic_any_t *o) {
  if (!it->node) {
    return -1;
  }

  *o = it->node->o;
  it->node = it->node->next;
  return 0;
}

void cosmic_list_iterator_close(cosmic_iterator_t *it) { free(it); }

struct cosmic_iterator_vtable COSMIC_LIST_ITERATOR_VTBL = {
    cosmic_list_iterator_next, NULL, cosmic_list_iterator_close};

cosmic_iterator_t *cosmic_list_iterator(const cosmic_list_t *l) {
  cosmic_iterator_t *it =
      (cosmic_iterator_t *)malloc(sizeof(struct cosmic_iterator));
  it->vtbl = &COSMIC_LIST_ITERATOR_VTBL;
  it->node = l->head;
  return it;
}