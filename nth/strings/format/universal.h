#ifndef NTH_STRINGS_FORMAT_UNIVERSAL_H
#define NTH_STRINGS_FORMAT_UNIVERSAL_H

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

// `nth::universal_formatter` is a formatter which is guaranteed to always
// print something (with a caveat discussed below), but for which the actualy
// contents formatted are not specified.
//
// The universal formatter may call into user-code, for example, if a
// user-defined type uses the `NthPrint` FTADLE hook. In such situations, it is
// possible that the user-code may not be SFINAE-friendly. It may therefore
// advertise that it can print things which it actually is not able to print,
// and therefore result in a compilation error when templates are instantiated.
// Barring this, however, attempting to use `nth::universal_formatter` should
// compile and print something regardless of the types passed to it.

// Prints some representation of `value` to `p`. There are no guarantees on the
// quality of representation, or any sort of specification for what will be
// printed. `UniversalPrint` may dispatch user-defined functions (for example,
// overloads of `operator<<` for accepting a `std::ostream&`). For such types,
// users are responsible for ensuring that any templates instantiated are
// properly constrained so that `requires` expressions querying for the validity
// of such expressions are accurate.
struct universal_formatter {
  struct options {
    // How many recursive instantiations of `universal_formatter::operator()`
    // should be followed at runtime before falling back to printing `fallback`.
    uint32_t depth = required;
    // The string to be printed as a fallback after we have followed `depth`
    // recursive instantiations of `UniversalPrint`.
    std::string_view fallback = required;
  };

  explicit constexpr universal_formatter(options opts) : options_(opts) {}

  void operator()(nth::Printer auto &p, auto const &value) {
    if (options_.depth == 0) {
      p.write(options_.fallback);
      return;
    }
    --options_.depth;
    IncrementOnDestruction cleanup(options_.depth);

    constexpr auto type = nth::type<
        std::remove_const_t<std::remove_reference_t<decltype(value)>>>;
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
        operator()(p, *value);
      } else {
        p.write("std::nullopt");
      }
    } else if constexpr (type.template is_a<std::chrono::time_point>()) {
      std::time_t t = std::chrono::system_clock::to_time_t(value);
      (*this)(p, std::put_time(std::localtime(&t), "%F %T"), options_);
    } else if constexpr (type.template is_a<std::variant>()) {
      std::visit([&](auto const &value) { operator()(p, value); }, value);
    } else if constexpr (requires { NthPrint(p, value); }) {
      NthPrint(p, value);
    } else if constexpr (
        std::is_array_v<std::remove_reference_t<decltype(value)>> or
        requires {
          value.begin();
          value.end();
        } or
        requires {
          begin(value);
          end(value);
        }) {
      p.write("{");
      std::string_view separator = "";
      for (auto const &element : value) {
        p.write(std::exchange(separator, ", "));
        operator()(p, element);
      }
      p.write("}");
    } else if constexpr (nth::tuple_like<nth::type_t<type>>) {
      p.write("{");
      constexpr size_t size = std::tuple_size_v<nth::type_t<type>>;
      std::apply(
          [&](auto &...elements) {
            std::string_view separator = "";
            ((p.write(std::exchange(separator, ", ")), operator()(p, elements)),
             ...);
          },
          [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            using ::std::get;
            return std::tie(get<Ns>(value)...);
          }(std::make_index_sequence<size>{}));
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
      p.write(1, ':');
      char buffer[4] = {' ', '\0', '\0', '\0'};
      for (char c : bytes) {
        uint8_t n = static_cast<uint8_t>(c);
        buffer[1] = HexDigits[n >> 4];
        buffer[2] = HexDigits[n & 0b1111];
        p.write(std::string_view(buffer));
      }
      p.write(1, ']');
    }
  }

 private:
  struct IncrementOnDestruction {
    explicit constexpr IncrementOnDestruction(uint32_t &n) : n_(n) {}
    ~IncrementOnDestruction() { ++n_; }

   private:
    uint32_t &n_;
  };
  options options_;
};

}  // namespace nth

#endif  // NTH_STRINGS_FORMAT_UNIVERSAL_H
