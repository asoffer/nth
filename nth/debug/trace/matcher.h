#ifndef NTH_DEBUG_TRACE_MATCHER_H
#define NTH_DEBUG_TRACE_MATCHER_H

#include <functional>
#include <string_view>
#include <tuple>
#include <utility>

#include "nth/debug/trace/internal/matcher_formatter.h"
#include "nth/debug/trace/trace.h"

namespace nth {

namespace internal_debug {

template <CompileTimeString, typename, typename...>
struct BoundExpectationMatcher;

}  // namespace internal_debug

template <CompileTimeString Name, typename F>
struct ExpectationMatcherImpl {
  explicit consteval ExpectationMatcherImpl(F f) : f_(std::move(f)) {}

  constexpr std::string_view name() const { return this->name_; }

  template <typename... Ts>
  constexpr internal_debug::BoundExpectationMatcher<Name, F, Ts...> operator()(
      Ts&&... values) const;

 private:
  template <CompileTimeString, typename, typename...>
  friend struct internal_debug::BoundExpectationMatcher;

  static constexpr std::string_view name_ = Name;
  F f_;
};

namespace internal_debug {

template <CompileTimeString Name, typename F, typename... Ts>
struct BoundExpectationMatcher : private ExpectationMatcherImpl<Name, F>,
                                 public BoundExpectationMatcherBase {
  using ExpectationMatcherImpl<Name, F>::name;

  auto operator()(auto const& value) const {
    return std::apply([&](auto const&... ts) { return this->f_(value, ts...); },
                      arguments_);
  }

  template <typename Fmt>
  friend void NthPrint(auto& p, MatcherFormatter<Fmt>& f,
                       BoundExpectationMatcher const& m) {
    f(p, m);
  }

 private:
  friend struct ::nth::ExpectationMatcherImpl<Name, F>;

  template <typename Fmt>
  friend struct MatcherFormatter;

  template <typename... Us>
  constexpr BoundExpectationMatcher(ExpectationMatcherImpl<Name, F> e, Us&&... us)
      : ExpectationMatcherImpl<Name, F>(e), arguments_(std::forward<Us>(us)...) {}

  std::string_view name_;
  std::tuple<Ts...> arguments_;
};

struct MatcherWrapBase {};

template <typename M, typename V>
struct MatcherWrap : MatcherWrapBase {
  operator auto() const { return m(v); }

  std::add_lvalue_reference_t<std::add_const_t<M>> m;
  std::add_lvalue_reference_t<std::add_const_t<V>> v;
};

template <CompileTimeString Name, typename F, typename... Ts>
auto operator>>=(nth::Traced auto const& value,
                 ExpectationMatcherImpl<Name, F> const& m) {
  return ::nth::internal_debug::MatcherWrap<decltype(m()), decltype(value)>{
      {}, m(), value};
}

template <CompileTimeString Name, typename F, typename... Ts>
auto operator>>=(nth::Traced auto const& value,
                 BoundExpectationMatcher<Name, F, Ts...> const& m) {
  return ::nth::internal_debug::MatcherWrap<decltype(m), decltype(value)>{
      {}, m, value};
}

template <typename M>
concept Matcher = std::derived_from<M, BoundExpectationMatcherBase>;

}  // namespace internal_debug

template <CompileTimeString Name, typename F>
template <typename... Ts>
constexpr internal_debug::BoundExpectationMatcher<Name, F, Ts...>
ExpectationMatcherImpl<Name, F>::operator()(Ts&&... values) const {
  return internal_debug::BoundExpectationMatcher<Name, F, Ts...>(
      *this, std::forward<Ts>(values)...);
}

template <CompileTimeString Name, typename F, typename... Ts>
bool Matches(
    internal_debug::BoundExpectationMatcher<Name, F, Ts...> const& matcher,
    auto const& value) {
  return matcher(value);
}

template <int&..., CompileTimeString Name, typename F>
bool Matches(ExpectationMatcherImpl<Name, F> const& matcher,
             auto const& value) {
  return nth::Matches(matcher(), value);
}

template <CompileTimeString Name, int&..., typename F>
consteval auto ExpectationMatcher(F&& f) {
  return ExpectationMatcherImpl<Name, F>(std::forward<F>(f));
}

bool Matches(auto const& match_value, auto const& value) {
  return match_value == value;
}

inline constexpr auto operator or(internal_debug::Matcher auto l,
                                  internal_debug::Matcher auto r) {
  return nth::ExpectationMatcher<"or">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) or r(value);
      })(l, r);
}

inline constexpr auto operator and(internal_debug::Matcher auto l,
                                   internal_debug::Matcher auto r) {
  return nth::ExpectationMatcher<"and">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) and r(value);
      })(l, r);
}

inline constexpr auto operator not(internal_debug::Matcher auto m) {
  return nth::ExpectationMatcher<"not">(
      [](auto const& value, auto const& m) { return not m(value); })(m);
}

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_MATCHER_H
