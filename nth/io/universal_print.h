#ifndef NTH_IO_UNIVERSAL_PRINT_H
#define NTH_IO_UNIVERSAL_PRINT_H

#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <string_view>
#include <utility>

#include "nth/io/printer.h"
#include "nth/meta/concepts.h"
#include "nth/meta/type.h"

namespace nth {

// Prints some representation of `value` to `p`. There are no guarantees on the
// quality of representation, or any sort of specification for what will be
// printed. `UniversalPrint` may dispatch user-defined functions (for example,
// overloads of `operator<<` for accepting a `std::ostream&`). For such types,
// users are responsible for ensuring that any templates instantiated are
// properly constrained so that `requires` expressions querying for the validity
// of such expressions are accurate.
void UniversalPrint(Printer auto &p, auto const &value) {
  constexpr auto type = nth::type<decltype(value)>.decayed();
  if constexpr (requires { p.write(value); } and
                (type == nth::type<char> or
                 not std::convertible_to<nth::type_t<type>, char>)) {
    p.write(value);
  } else if constexpr (type == nth::type<bool>) {
    p.write(value ? "true" : "false");
  } else if constexpr (requires {
                         {
                           std::declval<std::ostream &>() << value
                           } -> std::same_as<std::ostream &>;
                       }) {
    std::ostringstream out;
    out << value;
    p.write(out.str());
  } else if constexpr (nth::tuple_like<nth::type_t<type>>) {
    p.write("{");
    std::apply(
        [&](auto &...elements) {
          std::string_view separator = "";
          ((p.write(std::exchange(separator, ", ")),
            UniversalPrint(p, elements)),
           ...);
        },
        value);
    p.write("}");
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

}  // namespace nth

#endif  // NTH_IO_UNIVERSAL_PRINT_H
