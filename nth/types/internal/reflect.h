#ifndef NTH_TYPES_INTERNAL_REFLECT_H
#define NTH_TYPES_INTERNAL_REFLECT_H

#include <array>
#include <cstdarg>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <tuple>

#include "nth/memory/address.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace nth::internal_reflect {

template <size_t N>
struct any {
  template <typename T>
  operator T() const;
};

template <typename T, size_t N>
concept constructible_with = requires {
  []<size_t... Ns>(std::index_sequence<Ns...>) -> decltype((T{any<Ns>{}...}, true)) {
    return true;
  }(std::make_index_sequence<N>{});
};

enum ref_kind { lvalue, rvalue, const_lvalue, const_rvalue };

template <ref_kind K, auto Sequence>
struct field_ref_t {
  explicit field_ref_t(auto&&... args)
    requires(sizeof...(args) == Sequence.size())
      : fields_{const_cast<void*>(
            static_cast<void const*>(nth::raw_address(args)))...} {}

  template <size_t I>
  using type_at = nth::type_t<Sequence.template get<I>()>;

  template <size_t I>
    requires(K == ref_kind::lvalue)  //
  type_at<I>& get() {
    return *static_cast<type_at<I>*>(fields_[I]);
  }

  template <size_t I>
    requires(K == ref_kind::rvalue)  //
  type_at<I>&& get() {
    return static_cast<type_at<I>&&>(*static_cast<type_at<I>*>(fields_[I]));
  }

  template <size_t I>
    requires(K == ref_kind::const_lvalue)  //
  type_at<I>& get() {
    return *static_cast<type_at<I> const*>(fields_[I]);
  }

  template <size_t I>
    requires(K == ref_kind::const_rvalue)  //
  type_at<I>&& get() {
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

  } else if constexpr (Count == 6) {
    auto& [x0, x1, x2, x3, x4, x5] = v;
    return field_ref_t<
        K, nth::type_sequence<decltype(x0), decltype(x1), decltype(x2),
                              decltype(x3), decltype(x4), decltype(x5)>>(
        x0, x1, x2, x3, x4, x5);

  } else if constexpr (Count == 7) {
    auto& [x0, x1, x2, x3, x4, x5, x6] = v;
    return field_ref_t<
        K, nth::type_sequence<decltype(x0), decltype(x1), decltype(x2),
                              decltype(x3), decltype(x4), decltype(x5),
                              decltype(x6)>>(x0, x1, x2, x3, x4, x5, x6);
  }
}

template <typename T, int N>
constexpr int field_count_impl() {
  if constexpr (constructible_with<T, N>) {
    return field_count_impl<T, N + 1>();
  } else {
    return N - 1;
  }
}

template <typename T, int BaseCount>
constexpr int field_count() {
  if constexpr (requires {
                  { T::nth_field_count } -> nth::explicitly_convertible_to<int>;
                }) {
    return T::nth_field_count;
  } else if constexpr (std::is_aggregate_v<T>) {
    return field_count_impl<T, BaseCount>() - BaseCount;
  } else {
    return -1;
  }
}

struct parsing_state {
  int bases = 0;
  int depth = 0;
  int index = 0;
};

void field_name_getter(parsing_state& state, std::span<std::string_view> fields,
                       char const* format, ...);

inline void field_name_getter(parsing_state& state,
                              std::span<std::string_view> fields,
                              char const* format, ...) {
  std::va_list va;
  va_start(va, format);
  std::string_view fmt(format);
  if (fmt == " {\n") {
    ++state.depth;
    return;
  } else if (fmt == "%s}\n" or fmt == "}\n") {
    --state.depth;
    if (state.depth == 1 and state.bases != 0) { --state.bases; }
    return;
  }

  if (state.bases != 0 or state.depth != 1) { return; }
  va_arg(va, char const*);
  va_arg(va, char const*);
  fields[state.index++] = va_arg(va, char const*);
}

template <int FieldCount, typename T>
std::array<std::string_view, FieldCount> field_names(int base_count,
                                                     T const& value) {
  std::array<std::string_view, FieldCount> names;
  parsing_state state{.bases = base_count, .depth = 0};
  __builtin_dump_struct(nth::address(value), field_name_getter, state, names);
  return names;
}

}  // namespace nth::internal_reflect

template <size_t I, nth::internal_reflect::ref_kind K, auto Sequence>
struct std::tuple_element<I, nth::internal_reflect::field_ref_t<K, Sequence>> {
  using type = nth::type_t<Sequence.template get<I>()>;
};

template <nth::internal_reflect::ref_kind K, auto Sequence>
struct std::tuple_size<nth::internal_reflect::field_ref_t<K, Sequence>> {
  static constexpr size_t value = Sequence.size();
};

#endif  // NTH_TYPES_INTERNAL_REFLECT_H
