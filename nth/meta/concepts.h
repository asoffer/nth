#ifndef NTH_META_CONCEPTS_H
#define NTH_META_CONCEPTS_H

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>

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

// A concept matching any of the listed types.
template <typename T, typename... Ts>
concept any_of = (std::is_same_v<T, Ts> or ...);

// A concept matching any enumeration type (scoped or unscoped).
template <typename T>
concept enumeration = std::is_enum_v<T>;

// A concept matching any type that decays to `T`.
template <typename T, typename U>
concept decays_to = std::is_same_v<std::decay_t<T>, U>;

// A concept matching anything that can hash a `T`. Specifically, any object
// invocable with a `T` returning a type convertible to `size_t`.
template <typename H, typename T>
concept hasher = std::invocable<H, T> and
    std::convertible_to<std::invoke_result_t<H, T>, size_t>;

// A concept matching anything that satisfies the tuple protocol. Specifically,
// anything for which `std::tuple_size<T>::value` is a `size_t` constant
// expression, and for all `N` from zero up to (but excluding) this value,
// `typename std::tuple_element<N, T>::type` is a well-defined type.
template <typename T>
concept tuple_like = (requires {
  { std::tuple_size<T>::value } -> decays_to<size_t>;
} and internal_concepts::SpecializesAllStdTupleElements<T>);

}  // namespace nth

#endif  // NTH_META_CONCEPTS_H
