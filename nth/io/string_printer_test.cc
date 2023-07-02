#include "nth/io/string_printer.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(StringPrinter, Builtin) {
  std::string s;
  string_printer p(s);
  EXPECT_EQ(s, "");
  p.write('x');
  EXPECT_EQ(s, "x");
  p.write('y');
  EXPECT_EQ(s, "xy");
  p.write('z');
  EXPECT_EQ(s, "xyz");
  p.write(std::string_view("abc"));
  EXPECT_EQ(s, "xyzabc");
}

}  // namespace
}  // namespace nth
