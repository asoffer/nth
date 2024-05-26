#include "nth/meta/concepts/hash.h"

#include <cstddef>

struct NotAHasher {};
static_assert(not nth::hasher<NotAHasher, int>);

struct HashesIntNotPointer {
  size_t operator()(int);
};
static_assert(nth::hasher<HashesIntNotPointer, int>);
static_assert(nth::hasher<HashesIntNotPointer, bool>,
              "Implicit conversion from `bool` to `int`.");
static_assert(not nth::hasher<HashesIntNotPointer, int*>);

struct HashesBadReturn {
  int* operator()(int);
};
static_assert(not nth::hasher<HashesBadReturn, int>);
static_assert(not nth::hasher<HashesBadReturn, int*>);

struct S2 {
  operator size_t() const;
};
struct HashesConverting {
  S2 operator()(int) const;
};
static_assert(nth::hasher<HashesConverting, int>);

struct TemplateHasher {
  template <typename T>
  size_t operator()(T);
};

// Accepts a template
static_assert(nth::hasher<TemplateHasher, int>);
static_assert(nth::hasher<TemplateHasher, int*>);

int main() {}
