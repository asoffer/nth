#ifndef NTH_UTILITY_REQUIRED_H
#define NTH_UTILITY_REQUIRED_H

#include "nth/meta/type.h"

namespace nth {
namespace internal_required {

struct Impl {
  template <typename T>
  operator T() const {
    static_assert(::nth::type<T>.dependent(false), "This field is required.");
  }
};

}  // namespace internal_required

inline constexpr internal_required::Impl required;

}  // namespace nth

#endif  // NTH_UTILITY_REQUIRED_H
