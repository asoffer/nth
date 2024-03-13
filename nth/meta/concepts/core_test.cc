#include "nth/meta/concepts/core.h"

struct A {};
struct B {};
struct C : B {};

static_assert(nth::precisely<A, A>);
static_assert(not nth::precisely<A, B>);
static_assert(not nth::precisely<A, C>);
static_assert(not nth::precisely<B, C>);
static_assert(nth::precisely<A*, A*>);
static_assert(not nth::precisely<A*, B*>);
static_assert(not nth::precisely<A*, C*>);
static_assert(not nth::precisely<B*, C*>);

int main() {}
