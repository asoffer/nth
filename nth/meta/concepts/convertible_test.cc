#include "nth/meta/concepts/convertible.h"

#include <cstddef>

struct Base {};
struct Derived : Base {};
struct DerivedPrivately : private Base {};

static_assert(nth::explicitly_convertible_to<bool, int>);
static_assert(nth::explicitly_convertible_to<Base, Base>);
static_assert(nth::explicitly_convertible_to<Base, Derived>);
static_assert(not nth::explicitly_convertible_to<Base, DerivedPrivately>);

int main() {}
