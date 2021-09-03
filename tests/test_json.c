#include <assert.h>
#include <stdio.h>

#include <cosmic/cosmic_json.h>

void test_list_equals() {
  cosmic_json_t *l = cosmic_json_new_list();
  cosmic_json_add(l, cosmic_json_new_string("thomas"));
  cosmic_json_add(l, cosmic_json_new_string("thomas1"));
  cosmic_json_add(l, cosmic_json_new_string("thomas2"));

  cosmic_json_t *l_copy = cosmic_json_copy(l);

  assert(cosmic_json_equals(l, l_copy));

  cosmic_json_free(l);

  l = cosmic_json_new_list();
  cosmic_json_add(l, cosmic_json_new_string("thomas"));
  cosmic_json_add(l, cosmic_json_new_string("thomas1"));
  cosmic_json_add(l, cosmic_json_new_string("thomas2"));

  assert(cosmic_json_equals(l, l_copy));

  cosmic_json_remove(l, 1, NULL);
  assert(!cosmic_json_equals(l, l_copy));

  cosmic_json_free(l_copy);
  cosmic_json_free(l);
}

void test_object_equals() {
  cosmic_json_t *j = cosmic_json_new_object();
  cosmic_json_add_kv(j, "key", cosmic_json_new_string("thomas is a neat guy"));

  cosmic_json_t *j1 = cosmic_json_new_object();
  cosmic_json_add_kv(j1, "key", cosmic_json_new_string("thomas is a neat guy"));

  assert(cosmic_json_equals(j, j1));

  cosmic_json_free(j1);
  j1 = cosmic_json_copy(j);

  assert(cosmic_json_equals(j1, j));

  cosmic_json_add_kv(j, "another_key", cosmic_json_new_number(123));
  assert(!cosmic_json_equals(j, j1));

  cosmic_json_free(j);
  cosmic_json_free(j1);
}

void test_number_equals() {
  cosmic_json_t *n = cosmic_json_new_number(123);
  cosmic_json_t *n1 = cosmic_json_new_number(123);

  assert(cosmic_json_equals(n, n1));

  cosmic_json_free(n1);
  n1 = cosmic_json_new_number(321);

  assert(!cosmic_json_equals(n1, n));

  cosmic_json_free(n);
  cosmic_json_free(n1);
}

int main() {
  test_list_equals();
  test_object_equals();
  test_number_equals();
}

int main1() {
  cosmic_json_t *j1 = cosmic_json_new_object(), *j2 = cosmic_json_new_object();

  cosmic_json_add_kv(j1, "thomas", cosmic_json_new_number(11));

  cosmic_json_add_kv(j2, "thomas", cosmic_json_new_number(10));

  printf("%d\n", cosmic_json_equals(j1, j2));

  const cosmic_json_t *child = cosmic_json_get_object_value(j1, "thomas");
  if (child) {
    printf("%0.2f\n", cosmic_json_get_number(child));
  }

  cosmic_json_t *list = cosmic_json_new_list();
  cosmic_json_add(list, cosmic_json_new_string("lmember"));
  cosmic_json_add(list, cosmic_json_new_string("another one"));
  cosmic_json_add_kv(j1, "l", list);

  cosmic_iterator_t *it = cosmic_json_iterator(j1);
  cosmic_any_t o;
  while (cosmic_iterator_next(it, &o) == 0) {
    printf("loop %s\n", o.cp);
  }
  cosmic_iterator_close(it);

  cosmic_json_t *list_copy = cosmic_json_copy(list);
  int sz = cosmic_json_size(list_copy), i;
  for (i = 0; i < sz; i++) {
    const cosmic_json_t *c = cosmic_json_get_list_value(list_copy, i);
    printf("%d - %s\n", i, cosmic_json_get_string(c));
  }
  cosmic_json_free(list_copy);

  const cosmic_json_t *l = cosmic_json_get_object_value(j1, "l");
  const cosmic_json_t *ls = cosmic_json_get_list_value(l, 0);
  printf("%p %d %p %d\n", l, cosmic_json_get_type(l), ls, cosmic_json_size(l));
  puts(cosmic_json_get_string(ls));

  cosmic_json_remove_key(j1, "l", &list);
  cosmic_json_t *lstring = NULL;
  cosmic_json_remove(list, 0, &lstring);
  puts(cosmic_json_get_string(lstring));

  cosmic_json_remove_key(j1, "thomas", NULL);

  cosmic_json_replace_key(j2, "thomas", cosmic_json_new_string("another key"),
                          NULL);

  puts(cosmic_json_get_string(cosmic_json_get_object_value(j2, "thomas")));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
  cosmic_json_free(list);
  cosmic_json_free(lstring);

  return 0;
}