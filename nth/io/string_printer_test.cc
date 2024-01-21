#include "nth/io/string_printer.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("string_printer/builtin") {
  std::string s;
  string_printer p(s);
  NTH_EXPECT(s == "");
  p.write(1, 'x');
  NTH_EXPECT(s == "x");
  p.write(1, 'y');
  NTH_EXPECT(s == "xy");
  p.write(1, 'z');
  NTH_EXPECT(s == "xyz");
  p.write(std::string_view("abc"));
  NTH_EXPECT(s == "xyzabc");
}

}  // namespace
}  // namespace nth
