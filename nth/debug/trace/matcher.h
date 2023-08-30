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

template <CompileTimeString Name, typename F>
struct ParameterizedExpectationMatcher {
  explicit consteval ParameterizedExpectationMatcher(F f) : f_(std::move(f)) {}

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

template <CompileTimeString Name, typename F, typename... Ts>
struct BoundExpectationMatcher
    : private ParameterizedExpectationMatcher<Name, F>,
      public BoundExpectationMatcherBase {
  using ParameterizedExpectationMatcher<Name, F>::name;

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
  friend struct ::nth::internal_debug::ParameterizedExpectationMatcher<Name, F>;

  template <typename Fmt>
  friend struct MatcherFormatter;

  template <typename... Us>
  constexpr BoundExpectationMatcher(ParameterizedExpectationMatcher<Name, F> e,
                                    Us&&... us)
      : ParameterizedExpectationMatcher<Name, F>(e),
        arguments_(std::forward<Us>(us)...) {}

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
                 ParameterizedExpectationMatcher<Name, F> const& m) {
  return ::nth::internal_debug::MatcherWrap<decltype(m()), decltype(value)>{
      {}, m(), value};
}

template <CompileTimeString Name, typename F, typename... Ts>
auto operator>>=(nth::Traced auto const& value,
                 BoundExpectationMatcher<Name, F, Ts...> const& m) {
  return ::nth::internal_debug::MatcherWrap<decltype(m), decltype(value)>{
      {}, m, value};
}

}  // namespace internal_debug

template <CompileTimeString Name, int&..., typename F>
consteval auto ExpectationMatcher(F&& f) {
  return internal_debug::ParameterizedExpectationMatcher<Name, F>(
      std::forward<F>(f));
}

namespace internal_debug {

template <typename M>
concept Matcher = std::derived_from<M, BoundExpectationMatcherBase>;

inline constexpr auto operator or(Matcher auto l, Matcher auto r) {
  return nth::ExpectationMatcher<"or">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) or r(value);
      })(l, r);
}

inline constexpr auto operator and(Matcher auto l, Matcher auto r) {
  return nth::ExpectationMatcher<"and">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) and r(value);
      })(l, r);
}

inline constexpr auto operator not(Matcher auto m) {
  return nth::ExpectationMatcher<"not">(
      [](auto const& value, auto const& m) { return not m(value); })(m);
}

}  // namespace internal_debug

template <CompileTimeString Name, typename F>
template <typename... Ts>
constexpr internal_debug::BoundExpectationMatcher<Name, F, Ts...>
internal_debug::ParameterizedExpectationMatcher<Name, F>::operator()(
    Ts&&... values) const {
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
bool Matches(
    internal_debug::ParameterizedExpectationMatcher<Name, F> const& matcher,
    auto const& value) {
  return nth::Matches(matcher(), value);
}

bool Matches(auto const& match_value, auto const& value) {
  return match_value == value;
}

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_MATCHER_H
