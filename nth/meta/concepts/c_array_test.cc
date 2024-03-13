#include "nth/meta/concepts/c_array.h"

static_assert(nth::c_array<int[]>);
static_assert(nth::c_array<int[5]>);
static_assert(nth::c_array<int[4][5]>);
static_assert(not nth::c_array<int*>);

int main() {}
