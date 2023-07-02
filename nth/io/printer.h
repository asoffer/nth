#ifndef NTH_IO_PRINTER_H
#define NTH_IO_PRINTER_H

#include <concepts>
#include <string_view>

namespace nth {

template <typename T>
concept Printer = requires(T t) {
  { t.write(size_t{}, '0') } -> std::same_as<void>;
  { t.write(std::string_view()) } -> std::same_as<void>;
};

// `MinimalPrinter` is the most trivial type satisfying the `Printer` concept.
// It's member functions are not implemented as it is intended only for use at
// compile-time with type-traits. `MinimalPrinter` is intended to be used as a
// stand-in for compile-time checks where a generic printer is needed: If a
// member function can be instantiated with `MinimalPrinter`, it should be
// instantiatiable with any `Printer`.
struct MinimalPrinter {
  void write(std::same_as<size_t> auto, std::same_as<char> auto);
  void write(std::same_as<std::string_view> auto);
};

}  // namespace nth

#endif  // NTH_IO_PRINTER_H
