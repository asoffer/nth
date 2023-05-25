#include "nth/io/printer.h"

#include <string_view>

#include "gtest/gtest.h"

namespace nth {
namespace {

struct NotAPrinter {};

struct APrinter {
  void write(char);
  void write(std::string_view);
};

TEST(Printer, DistinguishesPrintersCorrectly) {
  constexpr bool n = Printer<NotAPrinter>;
  constexpr bool a = Printer<APrinter>;
  constexpr bool m = Printer<MinimalPrinter>;
  EXPECT_FALSE(n);
  EXPECT_TRUE(a);
  EXPECT_TRUE(m);
}

}  // namespace
}  // namespace nth
