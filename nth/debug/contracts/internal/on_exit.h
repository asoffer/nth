#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H

#include <concepts>
#include <utility>

#include "nth/debug/source_location.h"

namespace nth::debug::internal_contracts {

template <std::invocable<source_location> Fn>
struct OnExit {
  explicit constexpr OnExit(Fn &&f, source_location location)
      : f_(std::forward<Fn>(f)), location_(location) {}
  OnExit(OnExit const &)            = delete;
  OnExit(OnExit &&)                 = delete;
  OnExit &operator=(OnExit const &) = delete;
  OnExit &operator=(OnExit &&)      = delete;
  ~OnExit() { std::move(f_)(location_); }

 private:
  Fn f_;
  source_location location_;
};

template <typename Fn>
OnExit(Fn, source_location) -> OnExit<Fn>;

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
