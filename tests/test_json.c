#include "test_base.h"
#include <cosmic/cosmic_json.h>
#include <stdio.h>

void test_null_equals() {
  cosmic_json_t *j1 = cosmic_json_new_null();
  cosmic_json_t *j2 = cosmic_json_new_null();
  cosmic_json_t *j3 = cosmic_json_new_number(0);

  assert(cosmic_json_equals(j1, j2));
  assert(!cosmic_json_equals(j1, j3));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
  cosmic_json_free(j3);
}

void test_number_equals() {
  cosmic_json_t *j1 = cosmic_json_new_number(123);
  cosmic_json_t *j2 = cosmic_json_new_number(123);

  assert(cosmic_json_equals(j1, j2));

  cosmic_json_set_number(j2, 321);
  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  j1 = cosmic_json_new_string("thomas");
  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
}

void test_string_equals() {
  cosmic_json_t *j1 = cosmic_json_new_string("thomas");
  cosmic_json_t *j2 = cosmic_json_new_string("thomas");

  assert(cosmic_json_equals(j1, j2));

  cosmic_json_set_string(j1, "not thomas");
  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
}

void test_bool_equals() {
  cosmic_json_t *j1 = cosmic_json_new_bool(1);
  cosmic_json_t *j2 = cosmic_json_new_bool(0);

  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_set_bool(j2, 1);
  assert(cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
}

void test_object_equals() {
  cosmic_json_t *j1 = cosmic_json_new_object();
  cosmic_json_t *j2 = cosmic_json_new_object();

  cosmic_json_t *l1 = cosmic_json_new_list();

  cosmic_json_add_kv(j1, "key", cosmic_json_new_string("value"));
  cosmic_json_add_kv(j2, "key", cosmic_json_new_string("value"));

  assert(cosmic_json_equals(j1, j2));

  cosmic_json_add_kv(j1, "another_key", cosmic_json_new_string("value1"));

  assert(!cosmic_json_equals(j2, j1));

  cosmic_json_add_kv(j2, "another_key", cosmic_json_new_string("value1"));

  assert(cosmic_json_equals(j2, j1));

  cosmic_json_add_kv(j1, "number", cosmic_json_new_number(321));
  cosmic_json_add_kv(j2, "number", cosmic_json_new_number(321));

  assert(cosmic_json_equals(j1, j2));

  cosmic_json_remove_key(j1, "number", NULL);
  assert(!cosmic_json_equals(j1, j2));
  assert(cosmic_json_equals(j1, j1));

  cosmic_json_add_kv(j1, "number", cosmic_json_new_number(321));

  /**
   * Adding lists to object
   */

  cosmic_json_add(l1, cosmic_json_new_string("thomas"));
  cosmic_json_add(l1, cosmic_json_new_number(321));

  cosmic_json_add_kv(j1, "mylist", l1);
  assert(!cosmic_json_equals(j1, j2));
  cosmic_json_add_kv(j2, "mylist", cosmic_json_copy(l1));
  assert(cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
}

void test_list_equals() {
  cosmic_json_t *j1 = cosmic_json_new_list();
  cosmic_json_t *j2 = cosmic_json_new_list();
  cosmic_json_t* o1 = cosmic_json_new_object();

  assert(cosmic_json_equals(j1, j2));

  cosmic_json_add(j1, cosmic_json_new_string("thomas"));
  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_add(j2, cosmic_json_new_number(321));
  assert(!cosmic_json_equals(j2, j1));

  cosmic_json_remove(j2, 0, NULL);
  cosmic_json_add(j2, cosmic_json_new_string("thomas"));

  assert(cosmic_json_equals(j1, j2));

  /**
   * Add object to list
   */

  cosmic_json_add_kv(o1, "key", cosmic_json_new_string("value"));
  cosmic_json_add_kv(o1, "another_key", cosmic_json_new_string("value1"));
  cosmic_json_add_kv(o1, "number", cosmic_json_new_number(321));

  cosmic_json_add(j1, o1);
  assert(!cosmic_json_equals(j1, j2));

  /**
   * Remove key from o1 handle
   */
  cosmic_json_add(j2, cosmic_json_copy(o1));
  cosmic_json_remove_key(o1, "key", NULL);
  assert(!cosmic_json_equals(j1, j2));

  cosmic_json_free(j1);
  cosmic_json_free(j2);
}

int main() {
  test_null_equals();
  test_number_equals();
  test_bool_equals();
  test_object_equals();
  test_list_equals();
  return 0;
}
