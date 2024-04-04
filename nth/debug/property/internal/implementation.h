#ifndef NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H
#define NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H

#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "nth/debug/property/internal/concepts.h"
#include "nth/meta/compile_time_string.h"

namespace nth::debug {

template <compile_time_string Name, typename F, typename... Ts>
struct Property;

namespace internal_property {

template <compile_time_string Name, typename F>
struct ParameterizedProperty {
  explicit consteval ParameterizedProperty(F f) : f_(std::move(f)) {}

  constexpr std::string_view name() const { return this->name_; }

  template <typename... Ts>
  constexpr auto operator()(Ts&&... values) const {
    return Property<Name, F, std::remove_cvref_t<Ts>...>(
        *this, std::forward<Ts>(values)...);
  }

  auto invoke(auto const& value, auto const&... arguments) const {
    return f_(value, arguments...);
  }

 private:
  static constexpr std::string_view name_ = Name;
  F f_;
};

template <typename P, typename V>
struct PropertyWrap {
  operator auto() const { return property(value); }

  std::add_lvalue_reference_t<std::add_const_t<P>> property;
  std::add_lvalue_reference_t<std::add_const_t<V>> value;
};

}  // namespace internal_property
}  // namespace nth::debug

#endif  // NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H
