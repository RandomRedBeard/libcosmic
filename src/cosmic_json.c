#include "cosmic/cosmic_json.h"

struct cosmic_json {
  enum cosmic_json_type type;
  cosmic_any_t o;
};

/* Internals */

cosmic_json_t *cosmic_json_new(enum cosmic_json_type, cosmic_any_t);

void cosmic_json_error_ctor(cosmic_json_error_t *e,
                            enum cosmic_json_error error, ssize_t index,
                            const char *func_name) {
  e->error = error;
  e->index = index;
  e->func_name = func_name;
}

cosmic_json_t *cosmic_json_error_new(const cosmic_json_error_t re) {
  cosmic_json_error_t *e = malloc(sizeof(cosmic_json_error_t));
  *e = re;
  return cosmic_json_new(COSMIC_ERROR, COSMIC_ANY(e));
}

void cosmic_json_error_free(cosmic_json_error_t *e) { free(e); }

/* End internals */

int cosmic_json_raw_equals(const cosmic_json_t *j1, const cosmic_json_t *j2) {
  return j1->o.vp == j2->o.vp;
}

int cosmic_json_string_equals(const cosmic_json_t *j1,
                              const cosmic_json_t *j2) {
  return strcmp(j1->o.cp, j2->o.cp) == 0;
}

int cosmic_json_object_equals(const cosmic_json_t *j1,
                              const cosmic_json_t *j2) {
  cosmic_pair_t p;
  cosmic_any_t o;
  cosmic_map_t *m1 = j1->o.vp, *m2 = j2->o.vp;
  cosmic_iterator_t *it = NULL;

  if (cosmic_map_size(m1) != cosmic_map_size(m2)) {
    return 0;
  }

  it = cosmic_map_iterator(m1);
  while (cosmic_iterator_next_pair(it, &p) == 0) {
    if (cosmic_map_get_value(m2, p.k, &o) == -1) {
      cosmic_iterator_close(it);
      return 0;
    }

    if (!cosmic_json_equals(p.v.vp, o.vp)) {
      cosmic_iterator_close(it);
      return 0;
    }
  }

  cosmic_iterator_close(it);

  return 1;
}

int cosmic_json_list_equals(const cosmic_json_t *j1, const cosmic_json_t *j2) {
  cosmic_any_t o1, o2;
  cosmic_list_t *l1 = j1->o.vp, *l2 = j2->o.vp;
  cosmic_iterator_t *it1 = NULL, *it2 = NULL;

  if (cosmic_list_size(l1) != cosmic_list_size(l2)) {
    return 0;
  }

  it1 = cosmic_list_iterator(l1);
  it2 = cosmic_list_iterator(l2);

  while (cosmic_iterator_next(it1, &o1) == 0 &&
         cosmic_iterator_next(it2, &o2) == 0) {
    if (!cosmic_json_equals(o1.vp, o2.vp)) {
      cosmic_iterator_close(it1);
      cosmic_iterator_close(it2);
      return 0;
    }
  }

  cosmic_iterator_close(it1);
  cosmic_iterator_close(it2);

  return 1;
}

int cosmic_json_equals(const cosmic_json_t *j1, const cosmic_json_t *j2) {
  if (j1->type != j2->type) {
    return 0;
  }

  switch (j1->type) {
  case COSMIC_OBJECT:
    return cosmic_json_object_equals(j1, j2);
  case COSMIC_LIST:
    return cosmic_json_list_equals(j1, j2);
  case COSMIC_STRING:
    return cosmic_json_string_equals(j1, j2);
  default:
    return cosmic_json_raw_equals(j1, j2);
  }
}

enum cosmic_json_type cosmic_json_get_type(const cosmic_json_t *j) {
  return j->type;
}

ssize_t cosmic_json_size(const cosmic_json_t *j) {
  if (j->type != COSMIC_LIST && j->type != COSMIC_OBJECT) {
    return -1;
  }

  return j->type == COSMIC_LIST ? cosmic_list_size(j->o.vp)
                                : cosmic_map_size(j->o.vp);
}

/**
 * Get raw types
 * We'll do some type checks and return 0 for invalid
 */

CDOUBLE cosmic_json_get_number(const cosmic_json_t *j) {
  return j->type == COSMIC_NUMBER ? j->o.d : 0;
}

const char *cosmic_json_get_string(const cosmic_json_t *j) {
  return j->type == COSMIC_STRING ? j->o.cp : NULL;
}

CLONG cosmic_json_get_bool(const cosmic_json_t *j) {
  return j->type == COSMIC_BOOL ? j->o.l : 0;
}

const cosmic_json_error_t *cosmic_json_get_error(const cosmic_json_t *j) {
  return j->type == COSMIC_ERROR ? j->o.vp : NULL;
}

enum cosmic_json_error cosmic_json_get_error_code(const cosmic_json_t *j) {
  const cosmic_json_error_t *err = cosmic_json_get_error(j);
  return err ? err->error : COSMIC_NONE;
}

/**
 * Get complex types
 */

const cosmic_json_t *cosmic_json_get_object_value(const cosmic_json_t *root,
                                                  const char *k) {
  cosmic_json_t *v = NULL;

  if (root->type != COSMIC_OBJECT) {
    return NULL;
  }

  if (cosmic_map_get_value(root->o.vp, COSMIC_ANY_CONST(k), PCOSMIC_ANY(&v)) !=
      0) {
    return NULL;
  }

  return v;
}

CDOUBLE cosmic_json_get_object_value_n(const cosmic_json_t *root,
                                      const char *k) {
  const cosmic_json_t *j = cosmic_json_get_object_value(root, k);
  return j ? cosmic_json_get_number(j) : 0;
}

const char *cosmic_json_get_object_value_s(const cosmic_json_t *root,
                                           const char *k) {
  const cosmic_json_t *j = cosmic_json_get_object_value(root, k);
  return j ? cosmic_json_get_string(j) : NULL;
}

CLONG cosmic_json_get_object_value_b(const cosmic_json_t *root, const char *k) {
  const cosmic_json_t *j = cosmic_json_get_object_value(root, k);
  return j ? cosmic_json_get_bool(j) : 0;
}

const cosmic_json_t *cosmic_json_get_list_value(const cosmic_json_t *root,
                                                size_t i) {
  cosmic_json_t *v = NULL;

  if (root->type != COSMIC_LIST) {
    return NULL;
  }

  if (cosmic_list_get(root->o.vp, i, PCOSMIC_ANY(&v)) != 0) {
    return NULL;
  }

  return v;
}

/**
 * Set raw type
 */

int cosmic_json_set_number(cosmic_json_t *j, CDOUBLE d) {
  if (j->type != COSMIC_NUMBER) {
    return -1;
  }

  j->o.d = d;
  return 0;
}

int cosmic_json_set_string(cosmic_json_t *j, const char *c) {
  if (j->type != COSMIC_STRING || !c) {
    return -1;
  }

  free(j->o.cp);
  j->o.cp = strdup(c);
  return 0;
}

int cosmic_json_set_bool(cosmic_json_t *j, CLONG l) {
  if (j->type != COSMIC_BOOL) {
    return -1;
  }

  j->o.l = l;
  return 0;
}

/**
 * Skipping to new funcs
 */

cosmic_json_t *cosmic_json_new(enum cosmic_json_type type, cosmic_any_t o) {
  cosmic_json_t *j = malloc(sizeof(cosmic_json_t));

  j->type = type;
  j->o = o;
  return j;
}

cosmic_json_t *cosmic_json_new_null() {
  return cosmic_json_new(COSMIC_NULL, COSMIC_VOID);
}

cosmic_json_t *cosmic_json_new_number(CDOUBLE d) {
  return cosmic_json_new(COSMIC_NUMBER, COSMIC_ANY_D(d));
}

cosmic_json_t *cosmic_json_new_string(const char *c) {
  return c ? cosmic_json_new(COSMIC_STRING, COSMIC_ANY(strdup(c))) : NULL;
}

cosmic_json_t *cosmic_json_new_bool(CLONG l) {
  return cosmic_json_new(COSMIC_BOOL, COSMIC_ANY_L(l));
}

int cosmic_json_key_cmp(const cosmic_any_t o1, const cosmic_any_t o2) {
  return strcmp(o1.cp, o2.cp);
}

cosmic_json_t *cosmic_json_new_object() {
  return cosmic_json_new(COSMIC_OBJECT,
                         COSMIC_ANY(cosmic_dmap_new(cosmic_json_key_cmp)));
}

cosmic_json_t *cosmic_json_new_list() {
  return cosmic_json_new(COSMIC_LIST, COSMIC_ANY(cosmic_llist_new()));
}

cosmic_json_t *cosmic_json_copy_null() { return cosmic_json_new_null(); }

cosmic_json_t *cosmic_json_copy_number(const cosmic_json_t *j) {
  return cosmic_json_new_number(j->o.d);
}

cosmic_json_t *cosmic_json_copy_string(const cosmic_json_t *j) {
  return cosmic_json_new_string(j->o.cp);
}

cosmic_json_t *cosmic_json_copy_bool(const cosmic_json_t *j) {
  return cosmic_json_new_bool(j->o.l);
}

cosmic_json_t *cosmic_json_copy_object(const cosmic_json_t *j) {
  cosmic_pair_t p;
  cosmic_json_t *r = cosmic_json_new_object();
  cosmic_iterator_t *it = cosmic_map_iterator(j->o.vp);

  while (cosmic_iterator_next_pair(it, &p) == 0) {
    cosmic_json_add_kv(r, p.k.cp, cosmic_json_copy(p.v.vp));
  }
  cosmic_iterator_close(it);

  return r;
}

cosmic_json_t *cosmic_json_copy_list(const cosmic_json_t *j) {
  cosmic_any_t o;
  cosmic_json_t *r = cosmic_json_new_list();
  cosmic_iterator_t *it = cosmic_list_iterator(j->o.vp);

  while (cosmic_iterator_next(it, &o) == 0) {
    cosmic_json_add(r, cosmic_json_copy(o.vp));
  }
  cosmic_iterator_close(it);

  return r;
}

cosmic_json_t *cosmic_json_copy(const cosmic_json_t *j) {
  switch (j->type) {
  case COSMIC_NULL:
    return cosmic_json_copy_null();
  case COSMIC_NUMBER:
    return cosmic_json_copy_number(j);
  case COSMIC_STRING:
    return cosmic_json_copy_string(j);
  case COSMIC_BOOL:
    return cosmic_json_copy_bool(j);
  case COSMIC_OBJECT:
    return cosmic_json_copy_object(j);
  case COSMIC_LIST:
    return cosmic_json_copy_list(j);
  default: /* We don't need to copy errors */
    break;
  }

  return NULL;
}

void cosmic_json_map_dealloc(cosmic_pair_t p) {
  free(p.k.vp);
  cosmic_json_free(p.v.vp);
}

void cosmic_json_list_dealloc(cosmic_any_t o) { cosmic_json_free(o.vp); }

void cosmic_json_free(cosmic_json_t *j) {
  switch (j->type) {
  case COSMIC_OBJECT:
    cosmic_dmap_free(j->o.vp, cosmic_json_map_dealloc);
    break;
  case COSMIC_LIST:
    cosmic_llist_free(j->o.vp, cosmic_json_list_dealloc);
    break;
  case COSMIC_STRING:
    free(j->o.vp);
    break;
  case COSMIC_ERROR:
    cosmic_json_error_free(j->o.vp);
    break;
  default:
    break;
  }

  free(j);
}

ssize_t cosmic_json_add_kv(cosmic_json_t *root, const char *k,
                           cosmic_json_t *v) {
  /**
   * Type checking
   */
  if (root->type != COSMIC_OBJECT || v->type == COSMIC_ERROR) {
    return -1;
  }

  return cosmic_map_add(root->o.vp, COSMIC_ANY(strdup(k)), COSMIC_ANY(v));
}

ssize_t cosmic_json_add_kv_n(cosmic_json_t *root, const char *k, CDOUBLE d) {
  cosmic_json_t *j = cosmic_json_new_number(d);
  ssize_t i = cosmic_json_add_kv(root, k, j);
  if (i < 0) {
    cosmic_json_free(j);
  }
  return i;
}

ssize_t cosmic_json_add_kv_s(cosmic_json_t *root, const char *k,
                             const char *s) {
  cosmic_json_t *j = cosmic_json_new_string(s);
  ssize_t i = cosmic_json_add_kv(root, k, j);
  if (i < 0) {
    cosmic_json_free(j);
  }
  return i;
}

ssize_t cosmic_json_add_kv_b(cosmic_json_t *root, const char *k, CLONG l) {
  cosmic_json_t *j = cosmic_json_new_bool(l);
  ssize_t i = cosmic_json_add_kv(root, k, j);
  if (i < 0) {
    cosmic_json_free(j);
  }
  return i;
}

ssize_t cosmic_json_add(cosmic_json_t *list, cosmic_json_t *v) {
  if (list->type != COSMIC_LIST || v->type == COSMIC_ERROR) {
    return -1;
  }

  return cosmic_list_add(list->o.vp, COSMIC_ANY(v));
}

ssize_t cosmic_json_insert(cosmic_json_t *list, size_t i, cosmic_json_t *v) {
  if (list->type != COSMIC_LIST || v->type == COSMIC_ERROR) {
    return -1;
  }

  return cosmic_list_insert(list->o.vp, i, COSMIC_ANY(v));
}

int cosmic_json_remove_key(cosmic_json_t *j, const char *k,
                           cosmic_json_t **dest) {
  cosmic_pair_t p;
  int i;

  if (j->type != COSMIC_OBJECT) {
    return -1;
  }

  i = cosmic_map_remove(j->o.vp, COSMIC_ANY_CONST(k), &p);
  if (i == -1) {
    return i;
  }

  if (dest) {
    *dest = p.v.vp;
    free(p.k.cp);
  } else {
    cosmic_json_map_dealloc(p);
  }

  return i;
}

ssize_t cosmic_json_replace_key(cosmic_json_t *j, const char *k,
                                cosmic_json_t *r, cosmic_json_t **dest) {
  int i;
  if (r->type == COSMIC_ERROR) {
    return -1;
  }

  i = cosmic_json_remove_key(j, k, dest);
  if (i == -1) {
    return i;
  }

  return cosmic_json_add_kv(j, k, r);
}

int cosmic_json_remove(cosmic_json_t *j, size_t i, cosmic_json_t **dest) {
  cosmic_any_t o;
  int r;

  if (j->type != COSMIC_LIST) {
    return -1;
  }

  r = cosmic_list_remove(j->o.vp, i, &o);
  if (r == -1) {
    return r;
  }

  if (dest) {
    *dest = o.vp;
  } else {
    cosmic_json_free(o.vp);
  }

  return r;
}

cosmic_iterator_t *cosmic_json_iterator(const cosmic_json_t *j) {
  if (j->type != COSMIC_OBJECT && j->type != COSMIC_LIST) {
    return NULL;
  }

  return j->type == COSMIC_OBJECT ? cosmic_map_iterator(j->o.vp)
                                  : cosmic_list_iterator(j->o.vp);
}
