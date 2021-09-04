#ifndef COSMIC_JSON_H
#define COSMIC_JSON_H

#include <string.h>

#include <cosmic/cosmic_any.h>
#include <cosmic/cosmic_io.h>
#include <cosmic/cosmic_lib.h>
#include <cosmic/cosmic_list.h>
#include <cosmic/cosmic_map.h>

#ifdef __cplusplus
extern "C" {
#endif

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

COSMIC_DLL cosmic_json_t *cosmic_json_parse(const char *);
COSMIC_DLL cosmic_json_t *cosmic_json_parse_stream(cosmic_io_t *);

COSMIC_DLL int cosmic_json_write(cosmic_json_t *, char *, size_t);
COSMIC_DLL int cosmic_json_write_stream(cosmic_json_t *, cosmic_io_t *);

#ifdef __cplusplus
}
#endif

#endif
