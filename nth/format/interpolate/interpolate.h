#ifndef NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
#define NTH_FORMAT_INTERPOLATE_INTERPOLATE_H

#include <string>
#include <string_view>
#include <type_traits>

#include "nth/base/core.h"
#include "nth/format/format.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/writer/writer.h"

namespace nth {

template <interpolation_string S>
struct interpolating_formatter {};

template <interpolation_string S, int &..., io::writer W, typename... Ts>
void interpolate(W &w, Ts const &...values)
  requires(sizeof...(values) == S.placeholders())
{
  size_t start = 0;
  [&]<size_t... Ns>(std::index_sequence<Ns...>) {
    (
        [&] {
          constexpr auto r = S.template placeholder_range<Ns>();
          nth::io::write_text(w, static_cast<std::string_view>(S).substr(
                                     start, r.start - 1 - start));
          start    = r.start + r.length + 1;
          auto fmt = NthInterpolateFormatter<
              S.template unchecked_substr<r.start, r.length>()>(nth::type<Ts>);
          nth::format(w, fmt, values);
        }(),
        ...);

    nth::io::write_text(w, static_cast<std::string_view>(S).substr(start));
  }(std::make_index_sequence<S.placeholders()>{});
}

template <interpolation_string S, typename T>
auto NthInterpolateFormatter(type_tag<T>) {
  static_assert(S.empty());
  return default_formatter<T>();
}

template <interpolation_string S>
text_formatter NthInterpolateFormatter(type_tag<std::string>) {
  static_assert(S.empty());
  return {};
}

template <interpolation_string S>
text_formatter NthInterpolateFormatter(type_tag<std::string_view>) {
  static_assert(S.empty());
  return {};
}

template <interpolation_string S, size_t N>
text_formatter NthInterpolateFormatter(type_tag<char[N]>) {
  static_assert(S.empty());
  return {};
}

template <interpolation_string S, std::integral I>
auto NthInterpolateFormatter(type_tag<I>) {
  if constexpr (S.empty() or S == "d") {
    return base_formatter(10);
  } else if constexpr (S == "x") {
    return base_formatter(16);
  } else {
    constexpr bool Parse = S.empty();
    static_assert(Parse, "Failed to parse interpolation string");
  }
}

template <interpolation_string S>
auto NthInterpolateFormatter(type_tag<bool>) {
  if constexpr (S.empty() or S == "b") {
    return word_formatter<casing::lower>{};
  } else if constexpr (S == "B") {
    return word_formatter<casing::title>{};
  } else if constexpr (S == "B!") {
    return word_formatter<casing::upper>{};
  } else if constexpr (S == "d") {
    return base_formatter(10);
  } else {
    constexpr bool Parse = S.empty();
    static_assert(Parse, "Failed to parse interpolation string");
  }
}
}  // namespace nth

#endif  // NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
