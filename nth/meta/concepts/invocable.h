#ifndef NTH_META_CONCEPTS_INVOCABLE_H
#define NTH_META_CONCEPTS_INVOCABLE_H

#include "nth/meta/concepts/core.h"

namespace nth {

template <typename I, typename... Args>
using return_type =
    decltype(nth::value<I&>()(nth::value<nth::decayed<Args>>()...));

template <typename I, typename... Args>
concept invocable = requires {
  typename return_type<I, Args...>;
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_INVOCABLE_H
