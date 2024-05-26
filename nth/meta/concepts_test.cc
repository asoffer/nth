#include "nth/meta/concepts.h"

#include <tuple>
#include <type_traits>

namespace nth {
namespace {

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
