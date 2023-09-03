#ifndef NTH_DEBUG_TRACE_PROPERTY_PROPERTY_H
#define NTH_DEBUG_TRACE_PROPERTY_PROPERTY_H

#include <functional>
#include <string_view>
#include <tuple>
#include <utility>

#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/implementation.h"
#include "nth/debug/property/internal/property_formatter.h"
#include "nth/debug/trace/trace.h"

namespace nth {

// An `Property` represents a description of a property of a value to be
// tested/verified. As a concrete example, an `Property` might represent the
// property "the number is less than 5", or "the sequence is sorted."
template <CompileTimeString Name, typename F, typename... Ts>
struct Property : private internal_debug::ParameterizedProperty<Name, F> {
  using NthInternalIsDebugProperty = void;

  template <typename... Us>
  constexpr Property(internal_debug::ParameterizedProperty<Name, F> e,
                     Us&&... us)
      : internal_debug::ParameterizedProperty<Name, F>(e),
        arguments_(std::forward<Us>(us)...) {}

  using internal_debug::ParameterizedProperty<Name, F>::name;

  auto const& arguments() const { return arguments_; }

  auto operator()(auto const& value) const {
    return std::apply(
        [&](auto const&... ts) { return this->invoke(value, ts...); },
        arguments_);
  }

  template <typename Fmt>
  friend void NthPrint(auto& p, internal_debug::PropertyFormatterType auto& f,
                       Property const& m) {
    f(p, m);
  }

 private:
  std::string_view name_;
  std::tuple<Ts...> arguments_;
};

template <CompileTimeString Name, typename F, typename... Ts>
auto operator>>=(nth::Traced auto const& value,
                 Property<Name, F, Ts...> const& m) {
  return ::nth::internal_debug::PropertyWrap<decltype(m), decltype(value)>{
      m, value};
}

template <CompileTimeString Name, typename F, typename... Ts>
bool Matches(Property<Name, F, Ts...> const& matcher, auto const& value) {
  return matcher(value);
}

template <CompileTimeString Name, int&..., typename F>
consteval auto MakeProperty(F&& f) {
  return internal_debug::ParameterizedProperty<Name, F>(std::forward<F>(f));
}

bool Matches(auto const& match_value, auto const& value) {
  return match_value == value;
}

namespace internal_debug {

inline constexpr auto operator or(internal_debug::PropertyType auto l,
                                  internal_debug::PropertyType auto r) {
  return nth::MakeProperty<"or">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) or r(value);
      })(l, r);
}

inline constexpr auto operator and(internal_debug::PropertyType auto l,
                                   internal_debug::PropertyType auto r) {
  return nth::MakeProperty<"and">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) and r(value);
      })(l, r);
}

inline constexpr auto operator not(internal_debug::PropertyType auto m) {
  return nth::MakeProperty<"not">(
      [](auto const& value, auto const& m) { return not m(value); })(m);
}

}  // namespace internal_debug

inline constexpr auto LessThan = nth::MakeProperty<"less-than">(
    [](auto const& value, auto const& x) { return value < x; });

inline constexpr auto GreaterThan = nth::MakeProperty<"greater-than">(
    [](auto const& value, auto const& x) { return value > x; });

inline constexpr auto PointsTo =
    nth::MakeProperty<"points-to">([](auto const* value, auto const& element) {
      return value and nth::Matches(element, *value);
    });

inline constexpr auto ElementsAreSequentially =
    nth::MakeProperty<"elements-are (in order)">(
        [](auto const& value, auto const&... elements) {
          using std::begin;
          using std::end;
          auto&& v    = nth::EvaluateTraced(value);
          auto&& iter = begin(v);
          auto&& e    = end(v);
          return ((iter != e and nth::Matches(elements, *iter++)) and ...) and
                 iter == e;
        });

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_PROPERTY_PROPERTY_H
