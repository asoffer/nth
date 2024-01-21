#include "nth/io/printer.h"

#include <string_view>

#include "nth/test/test.h"

namespace nth {
namespace {

struct NotAPrinter {};

struct APrinter {
  void write(size_t, char);
  void write(std::string_view);
};

NTH_TEST("printer/distinguishes-printers-correctly") {
  constexpr bool n = Printer<NotAPrinter>;
  constexpr bool a = Printer<APrinter>;
  constexpr bool m = Printer<MinimalPrinter>;
  NTH_EXPECT(not n);
  NTH_EXPECT(a);
  NTH_EXPECT(m);
}

}  // namespace
}  // namespace nth
