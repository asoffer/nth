#ifndef NTH_TYPES_REFLECT_H
#define NTH_TYPES_REFLECT_H

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

#include "nth/base/core.h"
#include "nth/types/internal/reflect.h"

namespace nth {

// `nth::reflectable`:
//
// While many of the reflection capabilities built in to this library could be
// built on arbitrary types, a type owner must indicate that they desire
// reflection to be enabled for their type for these techniques to be allowed.
// This allows type owners to not become locked in to unintended behaviors
// simply because they happened to be observable (Hyrum's law).
template <typename T>
concept reflectable = requires { typename T::nth_reflectable; };

namespace reflect {

// Returns the number of fields in the struct `T`, under the assumption that `T`
// has `BaseCount` direct base classes (if this assumption is violated, behavior
// is undefined). If the struct contains a static data member named
// `nth_field_count`, that value is reported. Otherwise, the value reported will
// be either the number of fields in `T`, or `-1` (if the number cannot be
// determined).
template <reflectable T, int BaseCount>
constexpr int field_count = internal_reflect::field_count<T, BaseCount>();

template <int BaseCount, int&..., reflectable T>
std::array<std::string_view, ::nth::reflect::field_count<T, BaseCount>> const&
field_names(T const& value) {
  constexpr size_t FieldCount = ::nth::reflect::field_count<T, BaseCount>;
  static std::array<std::string_view, FieldCount> names =
      internal_reflect::field_names<FieldCount>(BaseCount, value);
  return names;
}

// Invokes `f(fields...)` where `fields` is a pack of references to the
// fields in the object `obj`.
template <int BaseCount, int&..., reflectable T>
auto on_fields(T const& obj, auto&& f) {
  if constexpr (::nth::reflect::field_count<T, BaseCount> == 0) {
    return NTH_FWD(f)();
  } else {
    auto refs =
        internal_reflect::get_fields<::nth::reflect::field_count<T, BaseCount>,
                                     internal_reflect::const_lvalue>(obj);
    return [&]<size_t... Ns>(std::index_sequence<Ns...>) {
      return NTH_FWD(f)(refs.template get<Ns>()...);
    }(std::make_index_sequence<::nth::reflect::field_count<T, BaseCount>>{});
  }
}

}  // namespace reflect
}  // namespace nth

#endif  // NTH_TYPES_REFLECT_H
