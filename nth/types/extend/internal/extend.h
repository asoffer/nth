#ifndef NTH_TYPES_DERVIE_INTERNAL_DERVIE_H
#define NTH_TYPES_DERVIE_INTERNAL_DERVIE_H

#include <concepts>

#include "nth/meta/type.h"
#include "nth/types/reflect.h"

namespace nth::internal_extend {

struct nonterminal {};

template <typename, typename>
struct extend_impl;

template <typename T, typename... Extensions>
struct extend_impl<T, void (*)(Extensions *...)> : Extensions... {
  using nth_reflectable = void;
};

constexpr auto all_extensions(void (*)(), auto p) { return p; }

template <typename T, typename... Unprocessed, typename... Processed>
constexpr auto all_extensions(void (*)(T *, Unprocessed...),
                              void (*p)(Processed...)) {
  auto next_unprocessed = static_cast<void (*)(Unprocessed...)>(nullptr);
  if constexpr (((nth::type<T *> == nth::type<Processed>) or ...)) {
    return internal_extend::all_extensions(next_unprocessed, p);
  } else if constexpr (std::derived_from<T, internal_extend::nonterminal>) {
    return internal_extend::all_extensions(
        static_cast<T::template concatenated<Unprocessed...>>(nullptr),
        static_cast<void (*)(T *, Processed...)>(nullptr));
  } else {
    return internal_extend::all_extensions(
        next_unprocessed, static_cast<void (*)(T *, Processed...)>(nullptr));
  }
}

}  // namespace nth::internal_extend

#endif  // NTH_TYPES_DERVIE_INTERNAL_DERVIE_H
