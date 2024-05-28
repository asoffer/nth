#ifndef NTH_IO_PRINTER_H
#define NTH_IO_PRINTER_H

#include <string_view>

#include "nth/meta/concepts/core.h"

namespace nth {

template <typename T>
concept Printer = requires(T t) {
  { t.write(size_t{}, '0') } -> nth::precisely<void>;
  { t.write(std::string_view()) } -> nth::precisely<void>;
};

// `MinimalPrinter` is the most trivial type satisfying the `Printer` concept.
// It's member functions are not implemented as it is intended only for use at
// compile-time with type-traits. `MinimalPrinter` is intended to be used as a
// stand-in for compile-time checks where a generic printer is needed: If a
// member function can be instantiated with `MinimalPrinter`, it should be
// instantiatiable with any `Printer`.
struct MinimalPrinter {
  void write(nth::precisely<size_t> auto, nth::precisely<char> auto);
  void write(nth::precisely<std::string_view> auto);
};

}  // namespace nth

#endif  // NTH_IO_PRINTER_H
