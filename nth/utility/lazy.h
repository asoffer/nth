#ifndef NTH_UTILITY_LAZY_H
#define NTH_UTILITY_LAZY_H

#include "nth/base/core.h"
#include "nth/meta/concepts/invocable.h"

namespace nth {

// Represents a lazily evaluated value. The type `lazy<F>` will invoke an object
// of type `F` with no arguments when converted to a value of whichever type `F`
// returns.
template <nth::invocable F>
requires(not nth::precisely<nth::return_type<F>, void>) struct lazy {
  lazy(F f) : f_(NTH_MOVE(f)) {}

  operator nth::return_type<F>() && { return NTH_FWD(f_)(); }

 private:
  F f_;
};

template <typename F>
lazy(F) -> lazy<F>;

}  // namespace nth

#endif  // NTH_UTILITY_LAZY_H
