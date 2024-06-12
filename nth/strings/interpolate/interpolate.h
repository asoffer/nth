#ifndef NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_INTERPOLATE_H

#include <concepts>
#include <string_view>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/internal/interpolate.h"
#include "nth/strings/interpolate/internal/parameter_tree.h"
#include "nth/strings/interpolate/string.h"
#include "nth/strings/interpolate/types.h"

namespace nth {
namespace internal_interpolate {

template <io::printer_type P>
struct interpolating_printer : io::printer<interpolating_printer<P>> {
  auto &inner() const NTH_ATTRIBUTE(lifetimebound) { return *printer_; }

  void write(std::string_view s) { printer_->write(s); }

  auto process_field(auto const &value) {
    size_t first_brace_index = interpolation_string_.find('{');
    inner().write(interpolation_string_.substr(0, first_brace_index));
    using T = std::remove_const_t<std::remove_reference_t<decltype(value)>>;
    if (interpolation_string_[first_brace_index + 1] == '}') {
      NthFormat(interpolating_printer("", inner()),
                nth::io::default_format_spec<T>(), value);
    } else {
      size_t matching_end_brace =
          matching_close(interpolation_string_, first_brace_index);
      std::string_view s = interpolation_string_.substr(
          first_brace_index + 1, matching_end_brace - first_brace_index);
      NthFormat(interpolating_printer(s, inner()),
                nth::format_spec_from<"", T>(), value);
    }

    interpolation_string_ = interpolation_string_.substr(
        matching_close(interpolation_string_, interpolation_string_.find('{')) +
        1);
    return *this;
  }

  template <int N>
  explicit interpolating_printer(interpolation_string<N> const &interp_str
                                     NTH_ATTRIBUTE(lifetimebound),
                                 P &printer NTH_ATTRIBUTE(lifetimebound))
      : interpolation_string_(interp_str), printer_(std::addressof(printer)) {}

  explicit interpolating_printer(std::string_view interp_str
                                     NTH_ATTRIBUTE(lifetimebound),
                                 P &printer NTH_ATTRIBUTE(lifetimebound))
      : interpolation_string_(interp_str), printer_(std::addressof(printer)) {}

  std::string_view interpolation_string_;
  P *printer_;
};

}  // namespace internal_interpolate

template <interpolation_string S, int &..., io::printer_type P>
void interpolate(P &p, auto const &...values) requires(sizeof...(values) ==
                                                       S.placeholders()) {
  if constexpr (sizeof...(values) == 0) {
    p.write(S);
  } else {
    constexpr internal_interpolate::interpolation_parameter_tree<S.count('{')>
        tree(S);
    [&]<size_t... Ns>(std::index_sequence<Ns...>) {
      ((p.write(std::string_view(S).substr(
            tree.template top_level_range<Ns - 1>().end() + 1,
            tree.template top_level_range<Ns>().start -
                tree.template top_level_range<Ns - 1>().end() - 2)),
        NthFormat(std::move(p),
                  nth::format_spec_from<
                      S.template unchecked_substr<
                          tree.template top_level_range<Ns>().start,
                          tree.template top_level_range<Ns>().length>(),
                      std::remove_cvref_t<decltype(values)>>(),
                  values)),
       ...);
    }
    (std::make_index_sequence<sizeof...(values)>{});

    p.write(std::string_view(S).substr(
        tree.template top_level_range<sizeof...(values) - 1>().end() + 1));
  }
}

template <interpolation_string S, int &..., io::structural_printer_type P>
void print_structure(P &p, auto const &...values) requires(sizeof...(values) ==
                                                           S.placeholders()) {
  if constexpr (sizeof...(values) != 0) {
    constexpr internal_interpolate::interpolation_parameter_tree<S.count('{')>
        tree(S);
    [&]<size_t... Ns>(std::index_sequence<Ns...>) {
      (NthFormat(std::move(p),
                 nth::format_spec_from<
                     S.template unchecked_substr<
                         tree.template top_level_range<Ns>().start,
                         tree.template top_level_range<Ns>().length>(),
                     std::remove_cvref_t<decltype(values)>>(),
                 values),
       ...);
    }
    (std::make_index_sequence<sizeof...(values)>{});
  }
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
