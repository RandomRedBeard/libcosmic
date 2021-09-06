#ifndef COSMIC_JSON_H
#define COSMIC_JSON_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <cosmic/cosmic_any.h>
#include <cosmic/cosmic_io.h>
#include <cosmic/cosmic_io_mem.h>
#include <cosmic/cosmic_lib.h>
#include <cosmic/cosmic_list.h>
#include <cosmic/cosmic_map.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COSMIC_NUMBER_BUFFER_LEN 32
#define COSMIC_STRING_MAX_LEN 1024
#define COSMIC_JSON_WHITESPACE " \n\t\r"

typedef struct cosmic_json cosmic_json_t;

enum cosmic_json_type {
  COSMIC_NULL,
  COSMIC_NUMBER,
  COSMIC_STRING,
  COSMIC_BOOL,
  COSMIC_OBJECT,
  COSMIC_LIST
};

COSMIC_DLL int cosmic_json_equals(const cosmic_json_t *, const cosmic_json_t *);
COSMIC_DLL enum cosmic_json_type cosmic_json_get_type(const cosmic_json_t *);
/**
 * Only applies to list
 */
COSMIC_DLL ssize_t cosmic_json_size(const cosmic_json_t *);

/**
 * New funcs
 */

COSMIC_DLL cosmic_json_t *cosmic_json_new_null();
COSMIC_DLL cosmic_json_t *cosmic_json_new_number(double);
COSMIC_DLL cosmic_json_t *cosmic_json_new_string(const char *);
COSMIC_DLL cosmic_json_t *cosmic_json_new_bool(long);
COSMIC_DLL cosmic_json_t *cosmic_json_new_object();
COSMIC_DLL cosmic_json_t *cosmic_json_new_list();

COSMIC_DLL cosmic_json_t *cosmic_json_copy(const cosmic_json_t *);

/**
 * Free Func
 */
COSMIC_DLL void cosmic_json_free(cosmic_json_t *);

/**
 * Get raw type
 */

COSMIC_DLL double cosmic_json_get_number(const cosmic_json_t *);
COSMIC_DLL const char *cosmic_json_get_string(const cosmic_json_t *);
COSMIC_DLL long cosmic_json_get_bool(const cosmic_json_t *);

/**
 * Get complex type
 */

COSMIC_DLL const cosmic_json_t *
cosmic_json_get_object_value(const cosmic_json_t *, const char *);
COSMIC_DLL const cosmic_json_t *
cosmic_json_get_list_value(const cosmic_json_t *, size_t);

/**
 * Set raw type
 */

COSMIC_DLL int cosmic_json_set_number(cosmic_json_t *, double);
COSMIC_DLL int cosmic_json_set_string(cosmic_json_t *, const char *);
COSMIC_DLL int cosmic_json_set_bool(cosmic_json_t *, long);

/**
 * Add key-value pair to object
 * Assumes ownership of json_t*
 */
COSMIC_DLL ssize_t cosmic_json_add_kv(cosmic_json_t *, const char *,
                                      cosmic_json_t *);
/**
 * Add object to list
 * Assumes ownership
 */
COSMIC_DLL ssize_t cosmic_json_add(cosmic_json_t *, cosmic_json_t *);
/**
 * Insert object to list
 * Assumes ownership
 */
COSMIC_DLL ssize_t cosmic_json_insert(cosmic_json_t *, size_t, cosmic_json_t *);

/**
 * Remove key and value from json object
 * If dest is not null, the value will be returned
 * otherwise it will be freed
 */
COSMIC_DLL int cosmic_json_remove_key(cosmic_json_t *, const char *,
                                      cosmic_json_t **);

/**
 * Performs remove and add on json object
 * If dest is not null, the old value is returned
 */
COSMIC_DLL ssize_t cosmic_json_replace_key(cosmic_json_t *, const char *,
                                           cosmic_json_t *, cosmic_json_t **);

/**
 * Remove value from json list
 * if dest is not null, the value will be returned
 * otherwise it will be freed
 */
COSMIC_DLL int cosmic_json_remove(cosmic_json_t *, size_t, cosmic_json_t **);

/**
 * Iterator is either map or list iterator
 * If type is map, pair type is {char*, cosmic_json_t*}
 */
COSMIC_DLL cosmic_iterator_t *cosmic_json_iterator(const cosmic_json_t *);

/**
 * Parsing and Writing functions
 */

COSMIC_DLL ssize_t cosmic_json_write_stream(const cosmic_json_t *,
                                            cosmic_io_t *, const char *);

/**
 * write_stream(io_mem_new(buf, len));
 */
COSMIC_DLL ssize_t cosmic_json_write_buffer(const cosmic_json_t *, char *,
                                            size_t, const char *);

COSMIC_DLL cosmic_json_t *cosmic_json_read_stream(cosmic_io_t *io);

/**
 * read_stream(io_mem_new(buf, len))
 */
COSMIC_DLL cosmic_json_t *cosmic_json_read_buffer(char *buf, size_t);

#ifdef USING_NAMESPACE_COSMIC

typedef cosmic_json_t json_t;

#define json_equals cosmic_json_equals
#define json_get_type cosmic_json_get_type
#define json_size cosmic_json_size
#define json_new_null cosmic_json_new_null
#define json_new_number cosmic_json_new_number
#define json_new_string cosmic_json_new_string
#define json_new_bool cosmic_json_new_bool
#define json_new_object cosmic_json_new_object
#define json_new_list cosmic_json_new_list
#define json_copy cosmic_json_copy
#define json_free cosmic_json_free
#define json_get_number cosmic_json_get_number
#define json_get_string cosmic_json_get_string
#define json_get_bool cosmic_json_get_bool
#define json_get_object_value cosmic_json_get_object_value
#define json_get_list_value cosmic_json_get_list_value
#define json_set_number cosmic_json_set_number
#define json_set_string cosmic_json_set_string
#define json_set_bool cosmic_json_set_bool
#define json_add_kv cosmic_json_add_kv
#define json_add cosmic_json_add
#define json_insert cosmic_json_insert
#define json_remove_key cosmic_json_remove_key
#define json_replace_key cosmic_json_replace_key
#define json_remove cosmic_json_remove
#define json_iterator cosmic_json_iterator
#define json_write_stream cosmic_json_write_stream
#define json_write_buffer cosmic_json_write_buffer
#define json_read_stream cosmic_json_read_stream
#define json_read_buffer cosmic_json_read_buffer

#endif

#ifdef __cplusplus
}
#endif

#endif
