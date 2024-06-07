#ifndef NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_INTERPOLATE_H

#include <concepts>
#include <string_view>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/string.h"
#include "nth/strings/interpolate/types.h"
#include "nth/strings/interpolate/internal/interpolate.h"

namespace nth {

template <interpolation_string S, io::printer_type P>
struct interpolating_printer : io::printer<interpolating_printer<S, P>> {
  friend constexpr auto NthFormatString(interpolating_printer const &) {
    return S;
  }

  auto& inner() const { return *printer_; }

  void write(std::string_view s) { printer_->write(s); }
  void write(size_t count, char c) { printer_->write(count, c); }

  auto process_field(auto const &value) {
    constexpr size_t first_brace_index = std::string_view(S).find('{');
    printer_->write(std::string_view(S).substr(0, first_brace_index));
    if constexpr (std::string_view(S)[first_brace_index + 1] == '}') {
      using inner_printer = interpolating_printer<
          nth::default_interpolation_string_for<std::decay_t<decltype(value)>>,
          P>;

      NthFormat(inner_printer(*printer_), value);

    } else {
      constexpr size_t matching_end_brace =
          internal_interpolate::matching_close(S, first_brace_index);
      using inner_printer = interpolating_printer<
          S.template unchecked_substr<first_brace_index + 1,
                                      matching_end_brace - first_brace_index>(),
          P>;

      NthFormat(inner_printer(*printer_), value);
    }

    return nth::type<interpolating_printer<
        S.template unchecked_substr<internal_interpolate::matching_close(
                                        S, std::string_view(S).find('{')) +
                                    1>(),
        P>>;
  }

  explicit interpolating_printer(P &printer NTH_ATTRIBUTE(lifetimebound))
      : printer_(std::addressof(printer)) {}

  P *printer_;
};

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
