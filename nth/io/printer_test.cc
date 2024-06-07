#include "nth/io/printer.h"

#include <string_view>

#include "nth/test/test.h"

namespace nth::io {
namespace {

struct not_a_printer : printer<not_a_printer> {};

struct a_printer : printer<a_printer> {
  void write(size_t, char);
  void write(std::string_view);
};

NTH_TEST("printer/distinguishes-printers-correctly") {
  constexpr bool n = printer_type<not_a_printer>;
  constexpr bool a = printer_type<a_printer>;
  constexpr bool m = printer_type<minimal_printer>;
  NTH_EXPECT(not n);
  NTH_EXPECT(a);
  NTH_EXPECT(m);
}

}  // namespace
}  // namespace nth::io
