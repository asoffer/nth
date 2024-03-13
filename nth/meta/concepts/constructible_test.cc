#include "nth/meta/concepts/constructible.h"

struct Trivial {};
struct ImplicitDefaultConstructor {
  ImplicitDefaultConstructor();
};

struct ExplicitDefaultConstructor {
  ExplicitDefaultConstructor();
};

struct Deleted {
  Deleted() = delete;
};

struct DerivesFromDeleted : Deleted {};

struct Indestructible {
  ~Indestructible() = delete;
};

static_assert(nth::default_constructible<Trivial>);
static_assert(nth::default_constructible<ImplicitDefaultConstructor>);
static_assert(nth::default_constructible<ExplicitDefaultConstructor>);
static_assert(not nth::default_constructible<Deleted>);
static_assert(not nth::default_constructible<DerivesFromDeleted>);
static_assert(nth::default_constructible<Indestructible>);

int main() {}
