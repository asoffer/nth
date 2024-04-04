#ifndef NTH_DEBUG_PROPERTY_PROPERTY_H
#define NTH_DEBUG_PROPERTY_PROPERTY_H

#include <functional>
#include <string_view>
#include <tuple>
#include <utility>

#include "nth/debug/property/internal/concepts.h"
#include "nth/debug/property/internal/implementation.h"
#include "nth/debug/trace/trace.h"
#include "nth/meta/concepts/invocable.h"

namespace nth::debug {

// A `Property` represents a description of a property of a value to be
// tested/verified. As a concrete example, an `Property` might represent the
// property "the number is less than 5", or "the sequence is sorted."
template <compile_time_string Name, typename F, typename... Ts>
struct Property
    : private debug::internal_property::ParameterizedProperty<Name, F> {
  using NthInternalIsDebugProperty = void;

  template <typename... Us>
  constexpr Property(internal_property::ParameterizedProperty<Name, F> e,
                     Us&&... us)
      : internal_property::ParameterizedProperty<Name, F>(e),
        arguments_(std::forward<Us>(us)...) {}

  using internal_property::ParameterizedProperty<Name, F>::name;

  auto const& arguments() const { return arguments_; }
  size_t argument_count() const { return sizeof...(Ts); }

  auto operator()(auto const& value) const {
    return std::apply(
        [&](auto const&... ts) { return this->invoke(value, ts...); },
        arguments_);
  }

  template <typename Fn>
  constexpr auto on_each_argument_reversed(Fn&& f) const
      requires((nth::invocable<Fn, Ts> and ...)) {
    std::apply(
        [&](auto const&... args) {
          [[maybe_unused]] int dummy;
          static_cast<void>((dummy = ... = (static_cast<void>(f(args)), 0)));
        },
        arguments_);
  }

 private:
  std::tuple<Ts...> arguments_;
};

// Used within an `NTH_EXPECT` or `NTH_ASSERT` macro, to test a value against a
// property. As an example, one might write
//
// ```
// NTH_EXPECT(my_vector >>= ElementsAreSequentially(GreaterThan(5),
//                                                  LessThan(3)));
// ```
//
// To test that `my_vector` contains exactly two elements, the first of which is
// greater than 5 and the second of which is less than three.
template <compile_time_string Name, typename F, typename... Ts>
auto operator>>=(auto const& value, Property<Name, F, Ts...> const& property) {
  return ::nth::debug::internal_property::PropertyWrap<
      Property<Name, F, Ts...> const&, decltype(value)>{property, value};
}

// Evaluates the `property` on the `value`, returning true if the property holds
// for the value and false otherwise.
template <compile_time_string Name, typename F, typename... Ts>
bool Matches(Property<Name, F, Ts...> const& property, auto const& value) {
  return property(value);
}

bool Matches(auto const& match_value, auto const& value) {
  return match_value == value;
}

// Constructs a `Property` from the given invocable `f`.
template <compile_time_string Name, int&..., typename F>
consteval auto MakeProperty(F&& f) {
  return internal_property::ParameterizedProperty<Name, std::remove_cvref_t<F>>(
      std::forward<F>(f));
}

inline constexpr auto operator or(internal_property::PropertyType auto l,
                                  internal_property::PropertyType auto r) {
  return nth::debug::MakeProperty<"or">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) or r(value);
      })(l, r);
}

inline constexpr auto operator and(internal_property::PropertyType auto l,
                                   internal_property::PropertyType auto r) {
  return nth::debug::MakeProperty<"and">(
      [](auto const& value, auto const& l, auto const& r) {
        return l(value) and r(value);
      })(l, r);
}

inline constexpr auto operator not(internal_property::PropertyType auto m) {
  return nth::debug::MakeProperty<"not">(
      [](auto const& value, auto const& m) { return not m(value); })(m);
}

inline constexpr auto LessThan = nth::debug::MakeProperty<"less-than">(
    [](auto const& value, auto const& x) { return value < x; });

inline constexpr auto GreaterThan = nth::debug::MakeProperty<"greater-than">(
    [](auto const& value, auto const& x) { return value > x; });

inline constexpr auto PointsTo = nth::debug::MakeProperty<"points-to">(
    [](auto const* value, auto const& element) {
      return value and nth::debug::Matches(element, *value);
    });

inline constexpr auto ElementsAreSequentially =
    nth::debug::MakeProperty<"elements-are (in order)">(
        [](auto const& value, auto const&... elements) {
          using std::begin;
          using std::end;
          auto&& v    = nth::debug::EvaluateTraced(value);
          auto&& iter = begin(v);
          auto&& e    = end(v);
          return ((iter != e and nth::debug::Matches(elements, *iter++)) and
                  ...) and
                 iter == e;
        });

}  // namespace nth::debug

#endif  // NTH_DEBUG_PROPERTY_PROPERTY_H
