#ifndef NTH_META_CONCEPTS_H
#define NTH_META_CONCEPTS_H

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace nth {

// A concept matching any of the listed types.
template <typename T, typename... Ts>
concept any_of = (std::is_same_v<T, Ts> or ...);

// A concept matching any enumeration type (scoped or unscoped).
template <typename T>
concept enumeration = std::is_enum_v<T>;

// A concept matching anything that can hash a `T`. Specifically, any object
// invocable with a `T` returning a type convertible to `size_t`.
template <typename H, typename T>
concept hasher = std::invocable<H, T> and
    std::convertible_to<std::invoke_result_t<H, T>, size_t>;

}  // namespace nth

#endif  // NTH_META_CONCEPTS_H
