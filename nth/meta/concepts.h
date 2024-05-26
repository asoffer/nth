#ifndef NTH_META_CONCEPTS_H
#define NTH_META_CONCEPTS_H

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>

#include "nth/meta/concepts/convertible.h"
#include "nth/meta/concepts/core.h"

namespace nth {
namespace internal_concepts {

template <typename T, size_t N>
concept SpecializesStdTupleElement = requires {
  typename std::tuple_element<N, T>::type;
};

template <typename T>
concept SpecializesAllStdTupleElements = ([
]<size_t... Ns>(std::index_sequence<Ns...>) {
  return (SpecializesStdTupleElement<T, Ns> and ...);
})(std::make_index_sequence<std::tuple_size<T>::value>{});

}  // namespace internal_concepts

// A concept matching anything that satisfies the tuple protocol. Specifically,
// anything for which `std::tuple_size<T>::value` is a `size_t` constant
// expression, and for all `N` from zero up to (but excluding) this value,
// `typename std::tuple_element<N, T>::type` is a well-defined type.
template <typename T>
concept tuple_like = (requires {
  { std::tuple_size<T>::value } -> decays_to<size_t>;
} and internal_concepts::SpecializesAllStdTupleElements<T>);

// A concept enabling types to opt-in to being "lvalue-reference-like." While
// there is no mechanism to properly copy the "decaying" behavior that
// distinguish C++ references from other types, types may be access to an object
// that exists at a memory location. All lvalue reference types meet the
// requirements of this concept. Additionally, types may opt-in to being treated
// as an `lvalue_proxy` by defining a nested type named `nth_lvalue_proxy`,
// and assigning to it the lvalue-reference type for which this is a proxy.
template <typename T>
concept lvalue_proxy = std::is_lvalue_reference_v<T> or requires {
  typename std::decay_t<T>::nth_lvalue_proxy;
  requires std::is_lvalue_reference_v<
      typename std::decay_t<T>::nth_lvalue_proxy>;
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_H
