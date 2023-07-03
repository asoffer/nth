#include "nth/io/string_printer.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(StringPrinter, Builtin) {
  std::string s;
  string_printer p(s);
  EXPECT_EQ(s, "");
  p.write(1, 'x');
  EXPECT_EQ(s, "x");
  p.write(1, 'y');
  EXPECT_EQ(s, "xy");
  p.write(1, 'z');
  EXPECT_EQ(s, "xyz");
  p.write(std::string_view("abc"));
  EXPECT_EQ(s, "xyzabc");
}

}  // namespace
}  // namespace nth
