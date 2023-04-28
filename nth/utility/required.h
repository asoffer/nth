#ifndef NTH_UTILITY_REQUIRED_H
#define NTH_UTILITY_REQUIRED_H

#include "nth/meta/type.h"

namespace nth {
namespace internal_required {

#if (defined(__clang__) and __clang_major__ >= 16) or defined(_MSC_VER)
#define NTH_INTERNAL_REQUIRED_EAGERLY_INSTANTIATION 0
#else 
#define NTH_INTERNAL_REQUIRED_EAGERLY_INSTANTIATION 1
#endif

struct Impl {
  template <typename T>
  operator T() const
#if NTH_INTERNAL_REQUIRED_EAGERLY_INSTANTIATION
      // On compilers that eagerly instantiate this template, we cannot provide
      // a definition (lest it be instantiated and therefore invoke the
      // static assertion. Instead we opt to leave it undefined so that the
      // linker complains about the missing implementation.
      ;
#else
  // On compilers that do not eagerly instantiate this template, we can provide
  // a useful diagnostic indicating that field is required.
  {
    static_assert(::nth::type<T>.dependent(false), "This field is required.");
  }
#endif
};

#undef NTH_INTERNAL_REQUIRED_EAGERLY_INSTANTIATION

}  // namespace internal_required

inline constexpr internal_required::Impl required;

}  // namespace nth

#endif  // NTH_UTILITY_REQUIRED_H
