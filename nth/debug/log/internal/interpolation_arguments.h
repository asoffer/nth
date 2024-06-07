#ifndef NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H

#include <array>
#include <memory>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/configuration/log.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate/interpolate.h"

namespace nth::internal_debug {

// An erased reference to an entry in `InterpolationArguments` (defined below).
struct ErasedInterpolationArgument {
  template <typename T>
  ErasedInterpolationArgument(NTH_ATTRIBUTE(lifetimebound) T const& t)
      : object_(std::addressof(t)),
        log_([](bounded_string_printer p, void const* t) {
          io::print(interpolating_printer<"{}", bounded_string_printer>(p),
                    *reinterpret_cast<T const*>(t));
        }) {}


  template <auto S>
  friend void NthFormat(interpolating_printer<S, bounded_string_printer> p,
                        ErasedInterpolationArgument e) {
    static constexpr std::string_view h = "hello";
    e.log_(p.inner(), &h);
    e.log_(std::move(p.inner()), e.object_);
  }
  friend void NthFormat(bounded_string_printer p,
                        ErasedInterpolationArgument e) {
    static constexpr std::string_view h = "hello";
    e.log_(p, &h);
    e.log_(std::move(p), e.object_);
  }

  void const* object_;
  void (*log_)(bounded_string_printer, void const*);
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

  template <typename Sink>
  InterpolationArgumentIgnorer& configure(Sink&,
                                          typename Sink::options const&) {
    return *this;
  }

  void operator=(InterpolationArgumentIgnorer const&) = delete;
  void operator=(InterpolationArgumentIgnorer&&) {}
  friend Voidifier operator<<=(InterpolationArgumentIgnorer const&, int) {
    return Voidifier();
  }
};
inline constexpr InterpolationArgumentIgnorer interpolation_argument_ignorer;

}  // namespace nth::internal_debug

#endif  // NTH_DEBUG_LOG_INTERNAL_INTERPOLATION_ARGUMENTS_H
