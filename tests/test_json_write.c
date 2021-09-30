#include "test_base.h"

#include <cosmic/cosmic_io_mem.h>
#include <cosmic/cosmic_json.h>

int main() {
  cosmic_io_t *io = NULL;
  cosmic_json_t *j = NULL, *j_copy = NULL, *l = NULL;
  char buf[1024],
      *expected_string =
          "{\"key\":\"value\",\"another_key\":\"value1\",\"number\":321.0000,"
          "\"obj\":{\"key\":\"value\",\"another_key\":\"value1\",\"number\":"
          "321.0000,\"list\":[123.0000,\"test "
          "123\",{\"key\":\"value\",\"another_key\":\"value1\",\"number\":321."
          "0000,\"obj\":{\"key\":\"value\",\"another_key\":\"value1\","
          "\"number\":321.0000,\"list\":[123.0000,\"test 123\"]}}]}}";

  memset(buf, 0, 1024);

  j = cosmic_json_new_object();
  cosmic_json_add_kv(j, "key", cosmic_json_new_string("value"));
  cosmic_json_add_kv(j, "another_key", cosmic_json_new_string("value1"));
  cosmic_json_add_kv(j, "number", cosmic_json_new_number(321));

  j_copy = cosmic_json_copy(j);
  cosmic_json_add_kv(j, "obj", j_copy);

  l = cosmic_json_new_list();
  cosmic_json_add(l, cosmic_json_new_number(123));
  cosmic_json_add(l, cosmic_json_new_string("test 123"));

  cosmic_json_add_kv(j_copy, "list", l);

  cosmic_json_add(l, cosmic_json_copy(j));

  io = cosmic_io_mem_new(buf, 1024);
  cosmic_json_write_stream_s(j, io, NULL, "%.4f");
  puts(buf);

  assert(strcmp(buf, expected_string) == 0);

  cosmic_json_free(j);
  cosmic_io_mem_free(io);
  return 0;
}
