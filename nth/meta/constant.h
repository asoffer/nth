#ifndef NTH_META_CONSTANT_H
#define NTH_META_CONSTANT_H

#include "nth/meta/type.h"

namespace nth {

// A value that can be used in an expression context but is guaranteed to be
// constant evaluated.
template <auto Value>
struct constant_value {
  static constexpr auto value = Value;

  // Returns the type of the underlying value.
  static constexpr auto type() {
    return nth::type<decltype(value)>.without_const();
  }

  // Casts the object to the underlying value.
  constexpr operator nth::type_t<type()>() const { return value; }
};

template <auto Value>
auto constant = constant_value<Value>();

}  // namespace nth

#endif  // NTH_META_CONSTANT_H
