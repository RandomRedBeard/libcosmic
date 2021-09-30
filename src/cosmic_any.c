#include "cosmic/cosmic_any.h"

cosmic_any_t COSMIC_ANY(void *v) {
  cosmic_any_t o;
  o.vp = v;
  return o;
}

cosmic_any_t COSMIC_ANY_CONST(const void *v) {
  cosmic_any_t o;
  o.cvp = v;
  return o;
}

cosmic_any_t *PCOSMIC_ANY(void *v) { return v; }

cosmic_any_t COSMIC_ANY_L(long l) {
  cosmic_any_t o;
  o.l = l;
  return o;
}

cosmic_any_t COSMIC_ANY_D(double d) {
  cosmic_any_t o;
  o.d = d;
  return o;
}

cosmic_any_t COSMIC_ANY_UL(unsigned long ul) {
  cosmic_any_t o;
  o.ul = ul;
  return o;
}

cosmic_pair_t COSMIC_PAIR(cosmic_any_t o1, cosmic_any_t o2) {
  cosmic_pair_t p;
  p.k = o1;
  p.v = o2;
  return p;
}
