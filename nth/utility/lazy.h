#ifndef NTH_UTILITY_LAZY_H
#define NTH_UTILITY_LAZY_H

#include <type_traits>
#include <utility>

namespace nth {

// Represents a lazily evaluated value. The type `lazy<F>` will invoke an object
// of type `F` with no arguments when converted to a value of whichever type `F`
// returns.
template <typename F>
requires(std::invocable<F> and
         not std::is_void_v<std::invoke_result_t<F>>) struct lazy {
  lazy(F f) : f_(std::forward<F>(f)) {}

  operator std::invoke_result_t<F>() && { return std::forward<F>(f_)(); }

 private:
  F f_;
};

template <typename F>
lazy(F) -> lazy<F>;

}  // namespace nth

#endif  // NTH_UTILITY_LAZY_H
