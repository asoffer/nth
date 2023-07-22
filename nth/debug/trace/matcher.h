#ifndef NTH_DEBUG_TRACE_MATCHER_H
#define NTH_DEBUG_TRACE_MATCHER_H

#include <functional>
#include <string_view>
#include <tuple>
#include <utility>

#include "nth/debug/trace/trace.h"

namespace nth {

namespace internal_debug {

template <typename, typename...>
struct BoundExpectationMatcher;

}  // namespace internal_debug

template <typename F>
struct ExpectationMatcher {
  explicit consteval ExpectationMatcher(std::string_view name, F f)
      : name_(name), f_(std::move(f)) {}

  consteval std::string_view name() const { return this->name_; }

  template <typename... Ts>
  constexpr internal_debug::BoundExpectationMatcher<F, Ts...> operator()(
      Ts&&... values) const;

 private:
  template <typename, typename...>
  friend struct internal_debug::BoundExpectationMatcher;

  std::string_view name_;
  F f_;
};

namespace internal_debug {

template <typename F, typename... Ts>
struct BoundExpectationMatcher : private ExpectationMatcher<F>,
                                 public BoundExpectationMatcherBase {
  using ExpectationMatcher<F>::name;

  auto operator()(auto const& value) const {
    return std::apply([&](auto const&... ts) { return this->f_(value, ts...); },
                      arguments_);
  }

  // private:
  template <typename... Us>
  constexpr BoundExpectationMatcher(ExpectationMatcher<F> e, Us&&... us)
      : ExpectationMatcher<F>(e), arguments_(std::forward<Us>(us)...) {}

  std::tuple<Ts...> arguments_;
};

template <typename F, typename... Ts>
auto operator>>=(nth::Traced auto const& value,
                 BoundExpectationMatcher<F, Ts...> const& m) {
  return m(value);
}

}  // namespace internal_debug

template <typename F>
template <typename... Ts>
constexpr internal_debug::BoundExpectationMatcher<F, Ts...>
ExpectationMatcher<F>::operator()(Ts&&... values) const {
  return internal_debug::BoundExpectationMatcher<F, Ts...>(
      *this, std::forward<Ts>(values)...);
}

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_MATCHER_H
