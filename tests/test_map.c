#include <assert.h>
#include <cosmic/cosmic_map.h>
#include <stdio.h>
#include <string.h>

cosmic_any_t cany_str(const char *c) { return COSMIC_ANY(strdup(c)); }

void cany_free(cosmic_any_t o) { free(o.vp); }

void cany_map_free(cosmic_pair_t p) {
  cany_free(p.k);
  cany_free(p.v);
}

void check_str(cosmic_any_t o, const char *c) {
  printf("Checking %s - %s\n", o.cp, c);
  assert(strcmp(o.cp, c) == 0);
}

void test_size() {
  cosmic_map_t *m = cosmic_map_new((cosmic_cmp)strcmp);

  cosmic_map_add(m, cany_str("key"), cany_str("value"));
  cosmic_map_add(m, cany_str("key1"), cany_str("value1"));

  assert(cosmic_map_size(m) == 2);

  assert(cosmic_map_add(m, COSMIC_ANY("key"), COSMIC_ANY("fail")) == -1);

  cosmic_map_free(m, cany_map_free);
}

void test_get() {
  cosmic_map_t *m = cosmic_map_new((cosmic_cmp)strcmp);

  cosmic_map_add(m, cany_str("key"), cany_str("value"));
  cosmic_map_add(m, cany_str("key1"), cany_str("value1"));
  cosmic_map_add(m, cany_str("key2"), cany_str("value2"));

  cosmic_any_t o;
  assert(cosmic_map_get_value(m, COSMIC_ANY_CONST("key"), &o) == 0);

  check_str(o, "value");

  assert(cosmic_map_get_value(m, COSMIC_ANY_CONST("not there"), NULL) == -1);

  assert(cosmic_map_get_value(m, COSMIC_ANY_CONST("key2"), &o) == 0);

  check_str(o, "value2");

  cosmic_map_free(m, cany_map_free);
}

void test_remove() {
  cosmic_map_t *m = cosmic_map_new((cosmic_cmp)strcmp);

  cosmic_map_add(m, cany_str("key"), cany_str("value"));
  cosmic_map_add(m, cany_str("key1"), cany_str("value1"));
  cosmic_map_add(m, cany_str("key2"), cany_str("value2"));
  cosmic_map_add(m, cany_str("key3"), cany_str("value3"));
  cosmic_map_add(m, cany_str("key4"), cany_str("value4"));
  cosmic_map_add(m, cany_str("key5"), cany_str("value5"));

  cosmic_pair_t p;
  assert(cosmic_map_remove(m, COSMIC_ANY_CONST("key5"), &p) == 0);

  check_str(p.k, "key5");
  check_str(p.v, "value5");

  cany_map_free(p);

  assert(cosmic_map_remove(m, COSMIC_ANY_CONST("key"), &p) == 0);

  check_str(p.k, "key");
  check_str(p.v, "value");

  cany_map_free(p);

  assert(cosmic_map_remove(m, COSMIC_ANY_CONST("not_key"), NULL) == -1);

  assert(cosmic_map_remove(m, COSMIC_ANY_CONST("key2"), &p) == 0);

  check_str(p.k, "key2");
  check_str(p.v, "value2");

  cany_map_free(p);

  cosmic_map_add(m, cany_str("key6"), cany_str("value6"));
  cosmic_map_add(m, cany_str("key7"), cany_str("value7"));

  assert(cosmic_map_size(m) == 5);

  assert(cosmic_map_remove(m, COSMIC_ANY_CONST("key3"), &p) == 0);

  check_str(p.k, "key3");
  check_str(p.v, "value3");

  cany_map_free(p);

  cosmic_map_free(m, cany_map_free);
}

void test_iterator() {
  cosmic_map_t *m = cosmic_map_new((cosmic_cmp)strcmp);

  cosmic_map_add(m, cany_str("key"), cany_str("value"));
  cosmic_map_add(m, cany_str("key1"), cany_str("value1"));
  cosmic_map_add(m, cany_str("key2"), cany_str("value2"));
  cosmic_map_add(m, cany_str("key3"), cany_str("value3"));
  cosmic_map_add(m, cany_str("key4"), cany_str("value4"));
  cosmic_map_add(m, cany_str("key5"), cany_str("value5"));

  cosmic_iterator_t *it = cosmic_map_iterator(m);
  cosmic_pair_t p;
  assert(cosmic_iterator_next_pair(it, &p) >= 0);

  check_str(p.k, "key");
  check_str(p.v, "value");

  cosmic_any_t o;
  assert(cosmic_iterator_next(it, &o) >= 0);

  check_str(o, "key1");

  assert(cosmic_iterator_next(it, &o) >= 0);

  check_str(o, "key2");

  cosmic_iterator_close(it);

  cosmic_map_free(m, cany_map_free);
}

int main() {
  test_size();
  test_get();
  test_remove();
  test_iterator();
  return 0;
}