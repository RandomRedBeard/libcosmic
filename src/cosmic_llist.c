#include "cosmic/cosmic_llist.h"
#include "internal/cosmic_iterator.h"
#include "internal/cosmic_list.h"

struct cosmic_llist_node {
  size_t i;
  cosmic_any_t o;
  struct cosmic_llist_node *next;
};

struct cosmic_list {
  const struct cosmic_list_vtable *vtbl;
  size_t len;
  struct cosmic_llist_node *head, *tail;
};

struct cosmic_iterator {
  const struct cosmic_iterator_vtable *vtbl;
  struct cosmic_llist_node *node;
};

struct cosmic_llist_node *cosmic_llist_node_new(size_t i, cosmic_any_t o) {
  struct cosmic_llist_node *n = malloc(sizeof(struct cosmic_llist_node));
  n->i = i;
  n->o = o;
  n->next = NULL;
  return n;
}

void cosmic_llist_node_free(struct cosmic_llist_node *n) { free(n); }

size_t cosmic_llist_size(const cosmic_list_t *l) { return l->len; }

struct cosmic_llist_node *cosmic_llist_get_node(const cosmic_list_t *l,
                                                size_t i) {
  struct cosmic_llist_node *n = l->head;
  if (i >= l->len) {
    return NULL;
  }

  while (n && n->i != i) {
    n = n->next;
  }

  return n;
}

int cosmic_llist_get(const cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  struct cosmic_llist_node *n = cosmic_llist_get_node(l, i);
  if (!n) {
    return -1;
  }
  *o = n->o;
  return 0;
}

ssize_t cosmic_llist_add(cosmic_list_t *l, cosmic_any_t o) {
  struct cosmic_llist_node *n = cosmic_llist_node_new(l->len++, o);

  if (!l->head) {
    l->head = l->tail = n;
    return l->len;
  }

  l->tail->next = n;
  l->tail = n;

  return l->len;
}

ssize_t cosmic_llist_insert(cosmic_list_t *l, size_t i, cosmic_any_t o) {
  struct cosmic_llist_node *n = NULL, *prev = NULL;
  if (i > l->len) {
    return -1;
  }

  /**
   * Insert at o on empty list
   */
  if (i == 0 && !l->head) {
    return cosmic_llist_add(l, o);
  }

  n = cosmic_llist_node_new(i, o);
  prev = i == 0 ? NULL : cosmic_llist_get_node(l, i - 1);

  if (i == 0) {
    n->next = l->head;
    l->head = n;
  } else {
    n->next = prev->next;
    prev->next = n;
  }

  /**
   * Tail check
   */
  if (prev == l->tail) {
    l->tail = n;
  }

  n = n->next;
  while (n) {
    n->i++;
    n = n->next;
  }

  return ++l->len;
}

int cosmic_llist_remove(cosmic_list_t *l, size_t i, cosmic_any_t *o) {
  struct cosmic_llist_node *n = NULL, *prev = NULL, *inc = NULL;
  if (i >= l->len) {
    return -1;
  }

  /**
   * Head case is different than other
   */
  if (i == 0) {
    n = l->head;
    l->head = l->head->next;
  } else {
    prev = cosmic_llist_get_node(l, i - 1);
    n = prev->next;
    prev->next = prev->next->next;
  }

  if (n == l->tail) {
    l->tail = prev;
  }

  /**
   * Decrement following nodes
   */
  inc = n;
  while (inc) {
    inc->i--;
    inc = inc->next;
  }

  /**
   * Decrement len
   */
  l->len--;

  *o = n->o;

  cosmic_llist_node_free(n);

  return 0;
}

int cosmic_llist_pop(cosmic_list_t *l, cosmic_any_t *o) {
  return cosmic_llist_remove(l, 0, o);
}

int cosmic_llist_iterator_next(cosmic_iterator_t *it, cosmic_any_t *o) {
  if (!it->node) {
    return -1;
  }

  *o = it->node->o;
  it->node = it->node->next;
  return 0;
}

int cosmic_llist_iterator_has_next(const cosmic_iterator_t *it) {
  return it->node != NULL;
}

void cosmic_llist_iterator_close(cosmic_iterator_t *it) { free(it); }

struct cosmic_iterator_vtable COSMIC_LLIST_ITERATOR_VTBL = {
    cosmic_llist_iterator_next, NULL, cosmic_llist_iterator_has_next,
    cosmic_llist_iterator_close};

cosmic_iterator_t *cosmic_llist_iterator(const cosmic_list_t *l) {
  cosmic_iterator_t *it = malloc(sizeof(struct cosmic_iterator));
  it->vtbl = &COSMIC_LLIST_ITERATOR_VTBL;
  it->node = l->head;
  return it;
}

struct cosmic_list_vtable COSMIC_LLIST_VTBL = {
    cosmic_llist_size,    cosmic_llist_get,    cosmic_llist_add,
    cosmic_llist_insert,  cosmic_llist_remove, cosmic_llist_pop,
    cosmic_llist_iterator};

cosmic_list_t *cosmic_llist_new() {
  cosmic_list_t *l = calloc(1, sizeof(cosmic_list_t));
  l->vtbl = &COSMIC_LLIST_VTBL;
  return l;
}

void cosmic_llist_free(cosmic_list_t *l, cosmic_list_dealloc dealloc) {
  cosmic_any_t o;

  while (cosmic_llist_pop(l, &o) >= 0) {
    if (dealloc) {
      dealloc(o);
    }
  }
  free(l);
}
