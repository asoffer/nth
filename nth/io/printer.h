#ifndef NTH_IO_PRINTER_H
#define NTH_IO_PRINTER_H

#include <concepts>
#include <string_view>

#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"

namespace nth {
namespace io {

namespace internal_printer {

template <typename P>
struct print_folder {
  explicit print_folder(P printer) : printer_(std::move(printer)) {}

  auto operator->*(auto const &value) {
    auto type = printer_.process_field(value);
    return print_folder<nth::type_t<type>>(
        nth::type_t<type>(*printer_.printer_));
  }

  P printer_;
};

struct flusher {
  friend void NthFormat(auto, flusher) {}
};

template <typename P>
void print(P &&p, auto const &...values) {
  (print_folder<P>(std::move(p))->*...->*values)->*flusher{};
}

}  // namespace internal_printer

template <typename P>
struct printer {
  static auto print(P p, auto const &...values) {
    internal_printer::print(std::move(p), values...);
  }
};

template <typename P>
concept printer_type = std::derived_from<P, printer<P>> and requires (P p) {
  { p.write(size_t{}, '0') } -> nth::precisely<void>;
  { p.write(std::string_view()) } -> nth::precisely<void>;
};

template <printer_type P>
void print(P p, auto const &...values) {
  P::print(std::move(p), values...);
}

// `minimal_printer` is the most trivial type satisfying the `printer_type`
// concept. Its member functions are not implemented as it is intended only for
// use at compile-time with type-traits. `minimal_printer` is intended to be
// used as a stand-in for compile-time checks where a generic printer is needed:
// If a member function can be instantiated with `minimal_printer`, it should be
// instantiatiable with any `printer_type`.
struct minimal_printer : printer<minimal_printer> {
  void write(nth::precisely<size_t> auto, nth::precisely<char> auto);
  void write(nth::precisely<std::string_view> auto);
};

}  // namespace io
}  // namespace nth

#endif  // NTH_IO_PRINTER_H
