#include "nth/meta/concepts.h"

#include <tuple>
#include <type_traits>

namespace nth {
namespace {

// any_of
static_assert(any_of<int, int>);
static_assert(not any_of<int, bool>);
static_assert(not any_of<int>);
static_assert(any_of<int, bool, int>);
static_assert(any_of<int, int, int>);
static_assert(not any_of<int, bool, char>);

enum E {};
struct S1 {};
enum class C {};
static_assert(enumeration<E>);
static_assert(not enumeration<S1>);
static_assert(enumeration<C>);

struct NotAHasher {};
static_assert(not hasher<NotAHasher, int>);

struct HashesIntNotPointer {
  size_t operator()(int);
};
static_assert(hasher<HashesIntNotPointer, int>);
static_assert(hasher<HashesIntNotPointer, bool>,
              "Implicit conversion from `bool` to `int`.");
static_assert(not hasher<HashesIntNotPointer, int*>);

struct HashesBadReturn {
  int* operator()(int);
};
static_assert(not hasher<HashesBadReturn, int>);
static_assert(not hasher<HashesBadReturn, int*>);

struct S2 {
  operator size_t();
};
struct HashesConverting {
  S2 operator()(int);
};
static_assert(hasher<HashesConverting, int>);

struct TemplateHasher {
  template <typename T>
  size_t operator()(T);
};

// Accepts a template
static_assert(hasher<TemplateHasher, int>);
static_assert(hasher<TemplateHasher, int*>);

// decays_to
static_assert(decays_to<int&, int>);
static_assert(decays_to<int const volatile&, int>);
static_assert(decays_to<void(), void (*)()>);
static_assert(decays_to<int[3], int*>);

struct TupleLike {
  int a, b, c;
};

struct DoesntSpecializeSize {
  int a, b, c;
};
struct MissesOneElementSpecialization {
  int a, b, c;
};

}  // namespace
}  // namespace nth

template <>
struct std::tuple_size<nth::TupleLike> : std::integral_constant<size_t, 3> {};

template <size_t N>
struct std::tuple_element<N, nth::TupleLike> {
  using type = int;
};

template <>
struct std::tuple_size<nth::MissesOneElementSpecialization>
    : std::integral_constant<size_t, 3> {};

template <>
struct std::tuple_element<0, nth::MissesOneElementSpecialization> {
  using type = int;
};

template <>
struct std::tuple_element<1, nth::MissesOneElementSpecialization> {
  using type = int;
};

template <size_t N>
struct std::tuple_element<N, nth::DoesntSpecializeSize> {
  using type = int;
};

namespace nth {
namespace {

static_assert(tuple_like<std::tuple<>>);
static_assert(tuple_like<std::tuple<int>>);
static_assert(tuple_like<std::tuple<int, bool>>);
static_assert(tuple_like<TupleLike>);
static_assert(not tuple_like<DoesntSpecializeSize>);
static_assert(not tuple_like<MissesOneElementSpecialization>);

}  // namespace
}  // namespace nth

int main() { return 0; }
