#include "cosmic/cosmic_map.h"

struct cosmic_map_node {
  cosmic_any_t k, v;
  struct cosmic_map_node *next, *prev;
};

struct cosmic_map {
  size_t len;
  struct cosmic_map_node *head, *tail;
  cosmic_cmp cmp;
};

struct cosmic_iterator_vtable {
  int (*next)(cosmic_iterator_t *, cosmic_any_t *);
  int (*next_pair)(cosmic_iterator_t *, cosmic_pair_t *);
  void (*close)(cosmic_iterator_t *);
};

struct cosmic_iterator {
  const struct cosmic_iterator_vtable *vtbl;
  struct cosmic_map_node *node;
};

struct cosmic_map_node *cosmic_map_node_new(cosmic_any_t k, cosmic_any_t v) {
  struct cosmic_map_node *n = malloc(sizeof(struct cosmic_map_node));
  n->k = k;
  n->v = v;
  n->next = n->prev = NULL;
  return n;
}

void cosmic_map_node_free(struct cosmic_map_node *n) { free(n); }

cosmic_map_t *cosmic_map_new(cosmic_cmp cmp) {
  cosmic_map_t *m = calloc(1, sizeof(cosmic_map_t));
  m->cmp = cmp;
  return m;
}

void cosmic_map_free(cosmic_map_t *m, cosmic_map_dealloc dealloc) {
  struct cosmic_map_node *prev, *n = m->head;
  cosmic_pair_t p;
  while (n) {
    if (dealloc) {
      p.k = n->k;
      p.v = n->v;
      dealloc(p);
    }
    prev = n;
    n = n->next;
    cosmic_map_node_free(prev);
  }
  free(m);
}

size_t cosmic_map_size(const cosmic_map_t *m) { return m->len; }

struct cosmic_map_node *cosmic_map_get_node(const cosmic_map_t *m,
                                            cosmic_any_t k) {
  struct cosmic_map_node *n = NULL;
  if (m->len == 0 || !m->head) {
    return NULL;
  }

  n = m->head;
  while (n) {
    if (m->cmp(n->k, k) == 0) {
      return n;
    }
    n = n->next;
  }

  return NULL;
}

int cosmic_map_get_value(const cosmic_map_t *m, cosmic_any_t k,
                         cosmic_any_t *v) {
  struct cosmic_map_node *n = cosmic_map_get_node(m, k);
  if (!n) {
    return -1;
  }

  *v = n->v;
  return 0;
}

ssize_t cosmic_map_add(cosmic_map_t *m, cosmic_any_t k, cosmic_any_t v) {
  struct cosmic_map_node *n = NULL;
  /**
   *  Fail on key exists
   */
  if (cosmic_map_get_node(m, k)) {
    return -1;
  }

  m->len++;

  n = cosmic_map_node_new(k, v);

  if (!m->head) {
    m->head = m->tail = n;
    return m->len;
  }

  m->tail->next = n;
  n->prev = m->tail;
  m->tail = n;

  return m->len;
}

struct cosmic_map_node *cosmic_map_remove_node(cosmic_map_t *m,
                                               cosmic_any_t k) {
  struct cosmic_map_node *n = cosmic_map_get_node(m, k);
  if (!n) {
    return NULL;
  }

  if (n->prev) {
    n->prev->next = n->next;
  }

  if (n->next) {
    n->next->prev = n->prev;
  }

  if (n == m->head) {
    m->head = n->next;
  }

  if (n == m->tail) {
    m->tail = n->prev;
  }

  m->len--;

  return n;
}

int cosmic_map_remove(cosmic_map_t *m, cosmic_any_t k, cosmic_pair_t *p) {
  struct cosmic_map_node *n = cosmic_map_remove_node(m, k);
  if (!n) {
    return -1;
  }

  p->k = n->k;
  p->v = n->v;

  cosmic_map_node_free(n);

  return 0;
}

int cosmic_map_iterator_next(cosmic_iterator_t *it, cosmic_any_t *o) {
  if (!it->node) {
    return -1;
  }

  *o = it->node->k;
  it->node = it->node->next;
  return 0;
}

int cosmic_map_iterator_next_pair(cosmic_iterator_t *it, cosmic_pair_t *p) {
  if (!it->node) {
    return -1;
  }

  p->k = it->node->k;
  p->v = it->node->v;

  it->node = it->node->next;
  return 0;
}

void cosmic_map_iterator_close(cosmic_iterator_t *it) { free(it); }

struct cosmic_iterator_vtable COSMIC_MAP_ITERATOR_VTBL = {
    cosmic_map_iterator_next, cosmic_map_iterator_next_pair,
    cosmic_map_iterator_close};

cosmic_iterator_t *cosmic_map_iterator(const cosmic_map_t *m) {
  cosmic_iterator_t *it = malloc(sizeof(struct cosmic_iterator));
  it->vtbl = &COSMIC_MAP_ITERATOR_VTBL;
  it->node = m->head;
  return it;
}
