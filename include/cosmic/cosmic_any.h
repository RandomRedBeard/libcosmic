#ifndef COSMIC_ANY_H
#define COSMIC_ANY_H

#ifdef __cplusplus
extern "C" {
#endif

#define COSMIC_ANY(o)                                                          \
  (cosmic_any_t) { .vp = o }
#define COSMIC_ANY_CONST(o)                                                    \
  (const cosmic_any_t) { .cvp = o }
#define PCOSMIC_ANY(o) (cosmic_any_t *)(void *) o
#define COSMIC_ANY_D(o)                                                        \
  (cosmic_any_t) { .d = o }
#define COSMIC_ANY_L(o)                                                        \
  (cosmic_any_t) { .l = o }
#define COSMIC_ANY_UL(o)                                                       \
  (cosmic_any_t) { .ul = o }

typedef union cosmic_any {
  double d;
  long l;
  unsigned long ul;
  char *cp;
  void *vp;
  const void *cvp; // Watch out
} cosmic_any_t;

typedef struct cosmic_pair {
  cosmic_any_t k, v;
} cosmic_pair_t;

// Wrapper for cmp func
typedef int (*cosmic_cmp)(const cosmic_any_t, const cosmic_any_t);

#ifdef __cplusplus
}
#endif

#endif