#ifndef NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H

#include <array>
#include <memory>

#include "nth/base/attributes.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/io/string_printer.h"

namespace nth::internal_debug {

// An erased reference to an entry in `InterpolationArguments` (defined below).
struct ErasedInterpolationArgument {
  template <typename T>
  ErasedInterpolationArgument(NTH_ATTRIBUTE(lifetimebound) T const& t)
      : object_(std::addressof(t)),
        log_([](bounded_string_printer& p, void const* t) {
          nth::config::default_formatter()(p, *reinterpret_cast<T const*>(t));
        }) {}

  friend void NthPrint(bounded_string_printer& p,
                       ErasedInterpolationArgument e) {
    e.log_(p, e.object_);
  }

  void const* object_;
  void (*log_)(bounded_string_printer& p, void const*);
};

// Arguments passed directly to a log macro via `operator<<=` as in the example:
//
// ```
// NTH_LOG((v.always), "{} + {} == {}) <<= {2, 3, 5};
// ```
//
template <int N>
struct InterpolationArguments {
  template <typename... Ts>
  InterpolationArguments(NTH_ATTRIBUTE(lifetimebound) Ts const&... ts)  //
      requires(sizeof...(Ts) == N)
      : entries_{ErasedInterpolationArgument(ts)...} {}

  auto const& entries() const { return entries_; };

 private:
  std::array<ErasedInterpolationArgument, N> entries_;
};

struct InterpolationArgumentIgnorer {
  constexpr InterpolationArgumentIgnorer()                          = default;
  InterpolationArgumentIgnorer(InterpolationArgumentIgnorer const&) = delete;
  InterpolationArgumentIgnorer(InterpolationArgumentIgnorer&&)      = delete;

  void operator=(InterpolationArgumentIgnorer const&) = delete;
  void operator=(InterpolationArgumentIgnorer&&) {}
  friend Voidifier operator<<=(InterpolationArgumentIgnorer const&, int) {
    return Voidifier();
  }
};
inline constexpr InterpolationArgumentIgnorer interpolation_argument_ignorer;

}  // namespace nth::internal_debug

#endif  // NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H
