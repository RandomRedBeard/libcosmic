#include "test_base.h"
#include <cosmic/cosmic_any.h>
#include <stdio.h>

void test_any() {
    unsigned int l = 120;
    cosmic_any_t o = COSMIC_ANY_L(l);
    assert(l == o.l);

    printf("%d - %lld\n", l, o.l);
}

int main() {
    test_any();
    return 0;
}