#ifndef NTH_IO_PRINTER_H
#define NTH_IO_PRINTER_H

#include <concepts>
#include <string_view>

namespace nth {

template <typename T>
concept Printer = requires(T t) {
  { t.write('0') } -> std::same_as<void>;
  { t.write(std::string_view()) } -> std::same_as<void>;
};

}  // namespace nth

#endif  // NTH_IO_PRINTER_H
