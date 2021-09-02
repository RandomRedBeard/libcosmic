#include <assert.h>
#include <cosmic/cosmic_map.h>
#include <stdio.h>
#include <string.h>

int main() {
  cosmic_map_t *m = cosmic_map_new((cosmic_cmp)strcmp);
  int i = cosmic_map_add(m, COSMIC_ANY("thomas"), COSMIC_ANY("jansen"));

  cosmic_any_t o;
  if (cosmic_map_get(m, COSMIC_ANY("thomas"), &o) == 0) {
    puts(o.cp);
  }

  cosmic_iterator_t *it = cosmic_map_iterator(m);
  cosmic_pair_t p;
  while (cosmic_iterator_next_pair(it, &p) == 0) {
    printf("%s - %s\n", p.k.cp, p.v.cp);
  }

  cosmic_iterator_close(it);

  cosmic_map_add(m, COSMIC_ANY("thomas1"), COSMIC_ANY("jansen"));
  cosmic_map_add(m, COSMIC_ANY("thomas2"), COSMIC_ANY("jansen"));

  cosmic_map_remove(m, COSMIC_ANY("thomas1"), &p);

  it = cosmic_map_iterator(m);
  while (cosmic_iterator_next_pair(it, &p) == 0) {
    printf("loop %s - %s\n", p.k.cp, p.v.cp);
  }

  cosmic_iterator_close(it);

  assert(cosmic_map_add(m, COSMIC_ANY("thomas"), COSMIC_ANY(NULL)));

  cosmic_map_free(m, NULL);

  return 0;
}