#ifndef NTH_META_CONCEPTS_HASH_H
#define NTH_META_CONCEPTS_HASH_H

#include <cstddef>

#include "nth/meta/concepts/convertible.h"
#include "nth/meta/concepts/invocable.h"

namespace nth {

// A concept matching anything that can hash a `T`. Specifically, any object
// invocable with a `T` returning a type convertible to `size_t`.
template <typename H, typename T>
concept hasher = nth::invocable<H, T> and
    nth::explicitly_convertible_to<nth::return_type<H, T>, size_t>;

}  // namespace nth

#endif  // NTH_META_CONCEPTS_HASH_H
