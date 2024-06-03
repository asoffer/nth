#ifndef NTH_TYPES_EXTEND_INTERNAL_EXTEND_H
#define NTH_TYPES_EXTEND_INTERNAL_EXTEND_H

#include <cstddef>
#include <tuple>

#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace nth::internal_extend {

template <template <typename> typename>
struct wrap {};

template <typename... Ts>
constexpr auto all_extensions(Ts...) {
  return static_cast<void (*)(Ts...)>(nullptr);
}

template <typename, typename...>
struct with_impl;

template <typename T, template <typename> typename... Extensions>
struct with_impl<T, void (*)(wrap<Extensions>...)> : Extensions<T>... {};

enum ref_kind { lvalue, rvalue, const_lvalue, const_rvalue };

template <ref_kind K, auto Sequence>
struct field_ref_t {
 private:
  template <size_t I>
  using type_at = nth::type_t<Sequence.template get<I>()>;

  template <size_t I>
  requires(K == ref_kind::lvalue)  //
      type_at<I>
  &get() { return *static_cast<type_at<I>*>(fields_[I]); }

  template <size_t I>
  requires(K == ref_kind::rvalue)  //
      type_at<I> && get() {
    return static_cast<type_at<I>&&>(*static_cast<type_at<I>*>(fields_[I]));
  }

  template <size_t I>
  requires(K == ref_kind::const_lvalue)  //
      type_at<I>
  &get() { return *static_cast<type_at<I> const*>(fields_[I]); }

  template <size_t I>
  requires(K == ref_kind::const_rvalue)  //
      type_at<I> && get() {
    return static_cast<type_at<I> const&&>(
        *static_cast<type_at<I> const*>(fields_[I]));
  }

 private:
  void* fields_[Sequence.size()];
};

template <size_t Count, ref_kind K>
auto get_fields(auto& v) {
  if constexpr (Count == 0) {
    return;
  } else if constexpr (Count == 1) {
    auto& [x0] = v;
    return field_ref_t<K, nth::type_sequence<decltype(x0)>>(x0);
  } else if constexpr (Count == 2) {
    auto& [x0, x1] = v;
    return field_ref_t<K, nth::type_sequence<decltype(x0), decltype(x1)>>(x0,
                                                                          x1);
  } else if constexpr (Count == 3) {
    auto& [x0, x1, x2] = v;
    return field_ref_t<
        K, nth::type_sequence<decltype(x0), decltype(x1), decltype(x2)>>(x0, x1,
                                                                         x2);
  } else if constexpr (Count == 4) {
    auto& [x0, x1, x2, x3] = v;
    return field_ref_t<K, nth::type_sequence<decltype(x0), decltype(x1),
                                             decltype(x2), decltype(x3)>>(
        x0, x1, x2, x3);

  } else if constexpr (Count == 5) {
    auto& [x0, x1, x2, x3, x4] = v;
    return field_ref_t<
        K, nth::type_sequence<decltype(x0), decltype(x1), decltype(x2),
                              decltype(x3), decltype(x4)>>(x0, x1, x2, x3, x4);
  }
}

template <typename>
auto type_sequence = nth::type_sequence<>;

}  // namespace nth::internal_extend

template <size_t I, nth::internal_extend::ref_kind K, auto Sequence>
struct std::tuple_element<I, nth::internal_extend::field_ref_t<K, Sequence>> {
  using type = nth::type_t<Sequence.template get<I>()>;
};

template <nth::internal_extend::ref_kind K, auto Sequence>
struct std::tuple_size<nth::internal_extend::field_ref_t<K, Sequence>> {
  static constexpr size_t value = Sequence.size();
};

#endif  // NTH_TYPES_EXTEND_INTERNAL_EXTEND_H
