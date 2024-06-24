#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H

#include "nth/debug/log/log.h"
#include "nth/debug/source_location.h"
#include "nth/format/interpolate/string.h"

namespace nth::internal_contracts {

template <typename Fn>
// Requires `Fn&&` is invocable with `source_location`. This requirement is not
// specified as a concept because all instantiations of this type are created
// internal to this library, meaning that we can avoid unnecessary `#include`s
// with no loss of user-visible safety.
struct OnExit {
  explicit constexpr OnExit(
      Fn f, source_location loc = nth::source_location::current())
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

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
