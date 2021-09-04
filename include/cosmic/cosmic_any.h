#ifndef COSMIC_ANY_H
#define COSMIC_ANY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cosmic/cosmic_lib.h>

typedef union cosmic_any {
  double d;
  long l;
  unsigned long ul;
  char *cp;
  void *vp;
  const void *cvp;
} cosmic_any_t;

typedef struct cosmic_pair {
  cosmic_any_t k, v;
} cosmic_pair_t;

/**
 *  Wrapper for cmp func
 */
typedef int (*cosmic_cmp)(const cosmic_any_t, const cosmic_any_t);

COSMIC_DLL cosmic_any_t COSMIC_ANY(void *);
COSMIC_DLL cosmic_any_t COSMIC_ANY_CONST(const void *);
COSMIC_DLL cosmic_any_t *PCOSMIC_ANY(void *);
COSMIC_DLL cosmic_any_t COSMIC_ANY_L(long);
COSMIC_DLL cosmic_any_t COSMIC_ANY_D(double);
COSMIC_DLL cosmic_any_t COSMIC_ANY_UL(unsigned long);

#ifdef __cplusplus
}
#endif

#endif
