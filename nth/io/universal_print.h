#ifndef NTH_IO_UNIVERSAL_PRINT_H
#define NTH_IO_UNIVERSAL_PRINT_H

#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string_view>
#include <utility>
#include <variant>

#include "nth/io/printer.h"
#include "nth/meta/concepts.h"
#include "nth/meta/type.h"
#include "nth/utility/required.h"

namespace nth {

struct UniversalPrintOptions {
  // How many recursive instantiations of `UniversalPrint` should be followed at
  // runtime before falling back to printing `fallback`.
  uint32_t depth = required;
  // The string to be printed as a fallback after we have followed `depth`
  // recursive instantiations of `UniversalPrint`.
  std::string_view fallback = required;
};

// Prints some representation of `value` to `p`. There are no guarantees on the
// quality of representation, or any sort of specification for what will be
// printed. `UniversalPrint` may dispatch user-defined functions (for example,
// overloads of `operator<<` for accepting a `std::ostream&`). For such types,
// users are responsible for ensuring that any templates instantiated are
// properly constrained so that `requires` expressions querying for the validity
// of such expressions are accurate.
void UniversalPrint(Printer auto &p, auto const &value,
                    UniversalPrintOptions options = {.depth    = 3,
                                                     .fallback = "..."});

namespace internal_universal_print {

void UniversalPrintImpl(Printer auto &p, auto const &value,
                        UniversalPrintOptions options) {
  if (options.depth == 0) {
    p.write(options.fallback);
  } else {
    --options.depth;
    UniversalPrint(p, value, options);
  }
}

}  // namespace internal_universal_print

void UniversalPrint(Printer auto &p, auto const &value,
                    UniversalPrintOptions options) {
  constexpr auto type = nth::type<decltype(value)>.decayed();
  if constexpr (type == nth::type<std::nullptr_t>) {
    p.write("nullptr");
  } else if constexpr (requires { p.write(value); } and
                       (type == nth::type<char> or
                        not std::convertible_to<nth::type_t<type>, char>)) {
    p.write(value);
  } else if constexpr (type == nth::type<bool>) {
    p.write(value ? "true" : "false");
  } else if constexpr (type == nth::type<std::nullopt_t>) {
    p.write("std::nullopt");
  } else if constexpr (type.template is_a<std::optional>()) {
    if (value.has_value()) {
      internal_universal_print::UniversalPrintImpl(p, *value, options);
    } else {
      internal_universal_print::UniversalPrintImpl(p, std::nullopt, options);
    }
  } else if constexpr (type.template is_a<std::chrono::time_point>()) {
    std::time_t t = std::chrono::system_clock::to_time_t(value);
    UniversalPrint(p, std::put_time(std::localtime(&t), "%F %T"), options);
  } else if constexpr (type.template is_a<std::variant>()) {
    std::visit(
        [&](auto const &value) {
          internal_universal_print::UniversalPrintImpl(p, value, options);
        },
        value);
  } else if constexpr (requires { NthPrint(p, value); }) {
    NthPrint(p, value);
  } else if constexpr (nth::tuple_like<nth::type_t<type>>) {
    p.write("{");
    std::apply(
        [&](auto &...elements) {
          std::string_view separator = "";
          ((p.write(std::exchange(separator, ", ")),
            internal_universal_print::UniversalPrintImpl(p, elements, options)),
           ...);
        },
        value);
    p.write("}");
  } else if constexpr (requires {
                         {
                           std::declval<std::ostream &>() << value
                           } -> std::same_as<std::ostream &>;
                       }) {
    std::ostringstream out;
    out << value;
    p.write(out.str());
  } else {
    static constexpr std::string_view HexDigits = "0123456789abcdef";
    std::string_view bytes(
        reinterpret_cast<char const *>(std::addressof(value)), sizeof(value));

    p.write("[Unprintable value of type ");
    p.write(type.name());
    p.write(':');
    for (char c : bytes) {
      p.write(' ');
      uint8_t n = static_cast<uint8_t>(c);
      p.write(HexDigits[n >> 4]);
      p.write(HexDigits[n & 0b1111]);
    }
    p.write("]");
  }
}

struct UniversalFormatter {
  void operator()(nth::Printer auto& p, auto const& v) const {
    nth::UniversalPrint(p, v);
  }
};
inline constexpr UniversalFormatter universal_formatter;

}  // namespace nth

#endif  // NTH_IO_UNIVERSAL_PRINT_H
