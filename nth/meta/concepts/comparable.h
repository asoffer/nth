#ifndef NTH_META_CONCEPTS_COMPARABLE_H
#define NTH_META_CONCEPTS_COMPARABLE_H

#include "nth/meta/concepts/core.h"

namespace nth {

template <typename T>
concept equality_comparable = requires(T const& t) {
  { t == t } -> nth::precisely<bool>;
  { t != t } -> nth::precisely<bool>;
};

template <typename T>
concept comparable = equality_comparable<T> and requires(T const& t) {
  { t < t } -> nth::precisely<bool>;
  { t <= t } -> nth::precisely<bool>;
  { t > t } -> nth::precisely<bool>;
  { t >= t } -> nth::precisely<bool>;
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_COMPARABLE_H
