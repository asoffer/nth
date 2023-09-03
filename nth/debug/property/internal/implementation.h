#ifndef NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H
#define NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H

#include <iostream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "nth/debug/property/internal/concepts.h"
#include "nth/meta/compile_time_string.h"

namespace nth {

template <CompileTimeString Name, typename F, typename... Ts>
struct Property;

namespace internal_debug {

template <CompileTimeString Name, typename F>
struct ParameterizedProperty {
  explicit consteval ParameterizedProperty(F f) : f_(std::move(f)) {}

  constexpr std::string_view name() const { return this->name_; }

  template <typename... Ts>
  constexpr auto operator()(Ts&&... values) const {
    return Property<Name, F, Ts...>(*this, std::forward<Ts>(values)...);
  }

  auto invoke(auto const& value, auto const&... arguments) const {
    return f_(value, arguments...);
  }

 private:
  static constexpr std::string_view name_ = Name;
  F f_;
};

template <typename M, typename V>
struct PropertyWrap {
  operator auto() const { return m(v); }

  std::add_lvalue_reference_t<std::add_const_t<M>> m;
  std::add_lvalue_reference_t<std::add_const_t<V>> v;
};

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_PROPERTY_INTERNAL_IMPLEMENTATION_H
