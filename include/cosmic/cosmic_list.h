#ifndef COSMIC_LIST_H
#define COSMIC_LIST_H

#include <stdlib.h>
#include <stdio.h>

#include <cosmic/cosmic_any.h>
#include <cosmic/cosmic_iterator.h>
#include <cosmic/cosmic_lib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cosmic_list cosmic_list_t;
typedef void (*cosmic_list_dealloc)(cosmic_any_t);

COSMIC_DLL cosmic_list_t *cosmic_list_new();
COSMIC_DLL void cosmic_list_free(cosmic_list_t *, cosmic_list_dealloc);
COSMIC_DLL size_t cosmic_list_size(const cosmic_list_t *);
COSMIC_DLL int cosmic_list_get(const cosmic_list_t *, size_t, cosmic_any_t *);
COSMIC_DLL ssize_t cosmic_list_add(cosmic_list_t *, cosmic_any_t);
COSMIC_DLL ssize_t cosmic_list_insert(cosmic_list_t *, size_t, cosmic_any_t);
COSMIC_DLL int cosmic_list_remove(cosmic_list_t *, size_t, cosmic_any_t *);
COSMIC_DLL int cosmic_list_pop(cosmic_list_t *, cosmic_any_t *);
COSMIC_DLL cosmic_iterator_t *cosmic_list_iterator(const cosmic_list_t *);

#ifdef __cplusplus
}
#endif

#endif
