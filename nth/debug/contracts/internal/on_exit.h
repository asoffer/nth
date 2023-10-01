#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H

#include <concepts>
#include <utility>

#include "nth/debug/log/log.h"
#include "nth/debug/source_location.h"
#include "nth/strings/interpolate.h"

namespace nth::debug::internal_contracts {

struct Logger {
  explicit Logger(source_location location) : loc_(location) {}
  template <::nth::InterpolationString S>
  void Log(auto const &...arguments) requires(sizeof...(arguments) ==
                                              S.placeholders()) {
    static ::nth::internal_debug::LogLineWithArity<sizeof...(arguments)> line(
        S, loc_);
    line <<= {arguments...};
  }

 private:
  template <std::invocable<source_location> Fn>
  friend struct OnExit;

  source_location loc_;
};

template <std::invocable<source_location> Fn>
struct OnExit {
  explicit constexpr OnExit(Fn &&f, source_location location)
      : f_(std::forward<Fn>(f)), logger_(location) {}
  OnExit(OnExit const &)            = delete;
  OnExit(OnExit &&)                 = delete;
  OnExit &operator=(OnExit const &) = delete;
  OnExit &operator=(OnExit &&)      = delete;
  ~OnExit() { std::move(f_)(logger_.loc_); }

 private:
  Fn f_;
  Logger logger_;
};

template <typename Fn>
OnExit(Fn, source_location) -> OnExit<Fn>;

}  // namespace nth::debug::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ON_EXIT_H
