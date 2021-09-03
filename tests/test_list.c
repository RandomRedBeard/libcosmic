#include <assert.h>
#include <cosmic/cosmic_list.h>
#include <stdio.h>
#include <string.h>

void print_list(cosmic_list_t *l) {
  cosmic_any_t o;
  cosmic_iterator_t *it = cosmic_list_iterator(l);
  while (cosmic_iterator_next(it, &o) == 0) {
    puts("loop");
    puts(o.cp);
  }

  cosmic_iterator_close(it);
}

int main() {
  cosmic_list_t *l = cosmic_list_new();
  cosmic_list_add(l, COSMIC_ANY(strdup("thomas is neat")));

  assert(cosmic_list_size(l) == 1);

  cosmic_any_t o;
  if (cosmic_list_get(l, 0, &o) == 0) {
    puts(o.cp);
  }

  if (cosmic_list_remove(l, 0, &o) == 0) {
    puts("Remove OK");
    free(o.cp);
  }

  cosmic_list_add(l, COSMIC_ANY(strdup("Thomas is very neat")));
  int i = cosmic_list_pop(l, &o);
  assert(i == 0);

  free(o.cp);

  cosmic_list_add(l, COSMIC_ANY(strdup("Thomas is very neat")));
  cosmic_list_add(l, COSMIC_ANY(strdup("Thomas also neat")));
  cosmic_list_insert(l, 0, COSMIC_ANY(strdup("FIRST")));
  cosmic_list_insert(l, 1, COSMIC_ANY(strdup("SECOND")));

  printf("%lu\n", cosmic_list_size(l));

  print_list(l);

  cosmic_list_remove(l, 2, &o);
  printf("Removed %s\n", o.cp);
  free(o.cp);

  print_list(l);

  cosmic_list_free(l, (cosmic_list_dealloc)free);
  return 0;
}