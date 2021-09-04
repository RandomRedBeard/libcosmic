#include "test_base.h"
#include <cosmic/cosmic_list.h>
#include <stdio.h>
#include <string.h>

cosmic_any_t cany_str(const char *c) { return COSMIC_ANY(strdup(c)); }

void cany_free(cosmic_any_t o) { free(o.vp); }

void check_str(cosmic_any_t o, const char *c) {
  printf("Checking %s - %s\n", o.cp, c);
  assert(strcmp(o.cp, c) == 0);
}

void get_and_check(cosmic_list_t *l, size_t i, const char *c) {
  cosmic_any_t o;
  assert(cosmic_list_get(l, i, &o) == 0);
  check_str(o, c);
}

void remove_and_check(cosmic_list_t *l, size_t i, const char *c) {
  cosmic_any_t o;
  assert(cosmic_list_remove(l, i, &o) == 0);
  check_str(o, c);
  cany_free(o);
}

void pop_and_check(cosmic_list_t *l, const char *c) {
  cosmic_any_t o;
  assert(cosmic_list_pop(l, &o) >= 0);
  check_str(o, c);
  cany_free(o);
}

void test_get() {
  cosmic_list_t *l = cosmic_list_new();
  cosmic_list_add(l, cany_str("thomas"));
  cosmic_list_add(l, cany_str("thomas1"));

  assert(cosmic_list_size(l) == 2);

  get_and_check(l, 1, "thomas1");

  cosmic_list_free(l, cany_free);
}

void test_insert() {
  cosmic_list_t *l = cosmic_list_new();
  cosmic_list_insert(l, 0, cany_str("thomas"));

  assert(cosmic_list_size(l) == 1);
  cosmic_list_add(l, cany_str("last"));

  get_and_check(l, 1, "last");
  cosmic_list_insert(l, 2, cany_str("newlast"));

  get_and_check(l, 2, "newlast");

  cosmic_list_insert(l, 0, cany_str("first"));
  get_and_check(l, 0, "first");

  assert(cosmic_list_insert(l, 10, COSMIC_ANY("NO_ALLOC")) == -1);

  cosmic_list_free(l, cany_free);
}

void test_remove() {
  cosmic_list_t *l = cosmic_list_new();

  cosmic_list_add(l, cany_str("time1"));
  cosmic_list_add(l, cany_str("time2"));
  cosmic_list_add(l, cany_str("time3"));
  cosmic_list_add(l, cany_str("time4"));

  remove_and_check(l, 0, "time1");
  remove_and_check(l, 2, "time4");

  cosmic_list_insert(l, 0, cany_str("first"));

  remove_and_check(l, 2, "time3");

  assert(cosmic_list_remove(l, 12, NULL) == -1);

  cosmic_list_free(l, cany_free);
}

void test_pop() {
  cosmic_list_t *l = cosmic_list_new();

  cosmic_list_add(l, cany_str("time1"));
  cosmic_list_add(l, cany_str("time2"));
  cosmic_list_add(l, cany_str("time3"));
  cosmic_list_add(l, cany_str("time4"));

  pop_and_check(l, "time1");
  pop_and_check(l, "time2");

  cosmic_list_free(l, cany_free);

  l = cosmic_list_new();
  assert(cosmic_list_pop(l, NULL) == -1);

  cosmic_list_free(l, NULL);
}

void test_iterator() {
  cosmic_any_t o;
  cosmic_list_t *l = cosmic_list_new();
  cosmic_iterator_t *it = NULL;

  cosmic_list_add(l, cany_str("time1"));
  cosmic_list_add(l, cany_str("time2"));
  cosmic_list_add(l, cany_str("time3"));
  cosmic_list_add(l, cany_str("time4"));

  it = cosmic_list_iterator(l);

  cosmic_iterator_next(it, &o);
  check_str(o, "time1");
  cosmic_iterator_next(it, &o);
  check_str(o, "time2");

  cosmic_iterator_close(it);

  cosmic_list_free(l, cany_free);
}

int main() {
  test_get();
  test_insert();
  test_remove();
  return 0;
}
