#include "test_base.h"

#include <cosmic/cosmic_json.h>

void test_read_object_number_string() {
  char *buf = "{ \"k\": 123, \"k1\": \"thomas\"}";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));
  const cosmic_json_t *child = NULL;
  assert(j);

  child = cosmic_json_get_object_value(j, "k");
  assert(cosmic_json_get_type(child) == COSMIC_NUMBER);
  assert(cosmic_json_get_number(child) == 123.00);

  cosmic_json_free(j);
}

void test_read_list() {
  char *buf =
      "     \n\n[\"thomas\", \"is\", \"named\", \"\\\"Mr. Jansen\\\"\", \"3\\\\4\"]";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));
  const cosmic_json_t *child = NULL;

  assert(j);

  child = cosmic_json_get_list_value(j, 3);
  assert(child);
  assert(cosmic_json_get_type(child) == COSMIC_STRING);
  assert(strcmp(cosmic_json_get_string(child), "\"Mr. Jansen\"") == 0);

  child = cosmic_json_get_list_value(j, 4);
  assert(child);
  assert(strcmp(cosmic_json_get_string(child), "3\\4") == 0);

  cosmic_json_free(j);
}

int main() {
  test_read_object_number_string();
  test_read_list();
  return 0;
}