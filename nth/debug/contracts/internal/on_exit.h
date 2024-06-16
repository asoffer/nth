#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H

#include "nth/debug/log/log.h"
#include "nth/debug/source_location.h"
#include "nth/strings/interpolate/string.h"

namespace nth::debug::internal_contracts {

template <compile_time_string File, int Line>
struct Logger {
  template <::nth::interpolation_string S>
  void Log(auto const &...arguments) requires(sizeof...(arguments) ==
                                              S.placeholders()) {
    internal_log::location_injector<__FILE__, __LINE__, __FUNCTION__, S>() <<=
        {arguments...};
  }
};

template <typename Fn>
// Requires `Fn&&` is invocable with `source_location`. This requirement is not
// specified as a concept because all instantiations of this type are created
// internal to this library, meaning that we can avoid unnecessary `#include`s
// with no loss of user-visible safety.
struct OnExit {
  explicit constexpr OnExit(Fn f, source_location loc)
      : f_(static_cast<Fn &&>(f)), loc_(loc) {}
  OnExit(OnExit const &)            = delete;
  OnExit(OnExit &&)                 = delete;
  OnExit &operator=(OnExit const &) = delete;
  OnExit &operator=(OnExit &&)      = delete;
  ~OnExit() { static_cast<Fn &&>(f_)(loc_); }

 private:
  Fn f_;
  source_location loc_;
};

template <typename Fn>
OnExit(Fn, source_location) -> OnExit<Fn>;

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
