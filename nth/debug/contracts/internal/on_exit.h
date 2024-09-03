#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H

#include "nth/debug/source_location.h"
#include "nth/format/interpolate.h"

namespace nth::internal_contracts {

template <typename Fn>
// Requires `Fn&&` is invocable with `source_location`. This requirement is not
// specified as a concept because all instantiations of this type are created
// internal to this library, meaning that we can avoid unnecessary `#include`s
// with no loss of user-visible safety.
struct on_exit {
  explicit constexpr on_exit(
      Fn f, source_location loc = nth::source_location::current())
      : f_(static_cast<Fn &&>(f)), loc_(loc) {}
  on_exit(on_exit const &)            = delete;
  on_exit(on_exit &&)                 = delete;
  on_exit &operator=(on_exit const &) = delete;
  on_exit &operator=(on_exit &&)      = delete;
  ~on_exit() { static_cast<Fn &&>(f_)(loc_); }

 private:
  Fn f_;
  source_location loc_;
};

template <typename Fn>
on_exit(Fn, source_location) -> on_exit<Fn>;

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
