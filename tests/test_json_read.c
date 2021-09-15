#include "test_base.h"

#include <cosmic/cosmic_json.h>

void test_read_object_number_string() {
  char *buf = "{ \"k\": 123, \"k1\": \"thomas\"}";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));
  const cosmic_json_t *child = NULL;
  assert(cosmic_json_get_error_code(j) == COSMIC_NONE);

  child = cosmic_json_get_object_value(j, "k");
  assert(cosmic_json_get_type(child) == COSMIC_NUMBER);
  assert(cosmic_json_get_number(child) == 123.00);

  cosmic_json_free(j);
}

void test_read_list() {
  char *buf = "     \n\n[\"thomas\", \"is\", \"named\", \"\\\"Mr. "
              "Jansen\\\"\", \"3\\\\4\"]";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));
  const cosmic_json_t *child = NULL;

  assert(cosmic_json_get_error_code(j) == COSMIC_NONE);

  child = cosmic_json_get_list_value(j, 3);
  assert(child);
  assert(cosmic_json_get_type(child) == COSMIC_STRING);
  assert(strcmp(cosmic_json_get_string(child), "\"Mr. Jansen\"") == 0);

  child = cosmic_json_get_list_value(j, 4);
  assert(child);
  assert(strcmp(cosmic_json_get_string(child), "3\\4") == 0);

  cosmic_json_free(j);
}

void test_io_bad() {
  char *buf = "{\"key\": ";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));

  assert(cosmic_json_get_type(j) == COSMIC_ERROR);
  assert(cosmic_json_get_error_code(j) == COSMIC_IO_ERROR);

  cosmic_json_free(j);
}

void test_invalid_read() {
  char *buf = "{\"key\": {123";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));

  assert(cosmic_json_get_type(j) == COSMIC_ERROR);
  assert(cosmic_json_get_error_code(j) == COSMIC_UNEXPECTED_CHAR);

  cosmic_json_free(j);
}

void test_depth_read() {
  char *buf = "[[[[[[[[[[[[[[";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));

  assert(cosmic_json_get_type(j) == COSMIC_ERROR);
  assert(cosmic_json_get_error_code(j) == COSMIC_MAX_DEPTH_ERROR);

  cosmic_json_free(j);
}

void test_invalid_read_prim() {
  char *buf = "{\"key\": nulS}";
  cosmic_json_t *j = cosmic_json_read_buffer(buf, strlen(buf));

  assert(cosmic_json_get_type(j) == COSMIC_ERROR);
  assert(cosmic_json_get_error_code(j) == COSMIC_UNEXPECTED_CHAR);

  const cosmic_json_error_t *err = cosmic_json_get_error(j);
  assert(err);

  /* Should identify where the error happened for buffered IO */
  assert(err->index == (strchr(buf, 'S') - buf));

  cosmic_json_free(j);
}

int main() {
  test_read_object_number_string();
  test_read_list();
  test_io_bad();
  test_invalid_read();
  test_depth_read();
  test_invalid_read_prim();
  return 0;
}
