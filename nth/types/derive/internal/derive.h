#ifndef NTH_TYPES_DERVIE_INTERNAL_DERVIE_H
#define NTH_TYPES_DERVIE_INTERNAL_DERVIE_H

#include <concepts>

#include "nth/format/format.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/type.h"
#include "nth/types/reflect.h"

namespace nth::internal_derive {

struct nonterminal {};

struct deriving_base {
  template <typename T>
  friend nth::trivial_format_spec NthFormatSpec(nth::interpolation_string_view,
                                                nth::type_tag<T>) {
    return {};
  }

  template <typename T>
  friend void NthFormat(nth::io::forward_writer auto &w,
                        nth::format_spec<T> const &, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    nth::format(w, {}, "{ ");
    std::string_view separator = "";
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      (nth::interpolate<"{}{} = {}">(w, std::exchange(separator, ", "),
                                     *name_ptr++, args),
       ...);
    });
    nth::format(w, {}, " }");
  }
};

template <typename>
struct deriving_impl;

template <typename... Extensions>
struct deriving_impl<void (*)(Extensions *...)> : deriving_base, Extensions... {
  using nth_reflectable = void;
};

constexpr auto all_extensions(void (*)(), auto p) { return p; }

template <typename T, typename... Unprocessed, typename... Processed>
constexpr auto all_extensions(void (*)(T *, Unprocessed...),
                              void (*p)(Processed...)) {
  auto next_unprocessed = static_cast<void (*)(Unprocessed...)>(nullptr);
  if constexpr (((nth::type<T *> == nth::type<Processed>) or ...)) {
    return internal_derive::all_extensions(next_unprocessed, p);
  } else if constexpr (std::derived_from<T, internal_derive::nonterminal>) {
    return internal_derive::all_extensions(
        static_cast<T::template concatenated<Unprocessed...>>(nullptr),
        static_cast<void (*)(T *, Processed...)>(nullptr));
  } else {
    return internal_derive::all_extensions(
        next_unprocessed, static_cast<void (*)(T *, Processed...)>(nullptr));
  }
}

}  // namespace nth::internal_derive

#endif  // NTH_TYPES_DERVIE_INTERNAL_DERVIE_H
