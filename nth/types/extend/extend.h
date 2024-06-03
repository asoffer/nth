#ifndef NTH_TYPES_EXTEND_EXTEND_H
#define NTH_TYPES_EXTEND_EXTEND_H

#include <memory>

#include "nth/meta/always_false.h"
#include "nth/types/extend/internal/extend.h"

namespace nth {

// `nth::extend` is a mechanism for mixing behavior into a struct/class. To use
// extensions with a type `MyType`, it must inherit publicly from
// `nth::extend<MyType>::with<Extensions...>` where `Extensions` is a pack of
// extensions whose behavior should be mixed in to `MyType`.
template <typename T>
struct extend final {
  template <template <typename> typename... Extensions>
  struct with : internal_extend::with_impl<
                    T, decltype(internal_extend::all_extensions(
                           internal_extend::wrap<Extensions>{}...))> {};
  extend()  = delete;
  ~extend() = delete;

 private:
  static constexpr internal_extend::field_ref_t<
      internal_extend::lvalue, internal_extend::type_sequence<T>>
  fields(T& t) {
    return internal_extend::get_fields<T::nth_field_count, T>(t);
  }
  static constexpr internal_extend::field_ref_t<
      internal_extend::const_lvalue, internal_extend::type_sequence<T>>
  fields(T const& t) {
    return internal_extend::get_fields<T::nth_field_count, T>(t);
  }
  static constexpr internal_extend::field_ref_t<
      internal_extend::rvalue, internal_extend::type_sequence<T>>
  fields(T&& t) {
    return internal_extend::get_fields<T::nth_field_count, T>(t);
  }
  static constexpr internal_extend::field_ref_t<
      internal_extend::const_rvalue, internal_extend::type_sequence<T>>
  fields(T const&& t) {
    return internal_extend::get_fields<T::nth_field_count, T>(t);
  }
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_H
