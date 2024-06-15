#include "nth/strings/interpolate/interpolate.h"

#include "nth/io/string_printer.h"
#include "nth/meta/type.h"
#include "nth/test/raw/test.h"

namespace {

void Simple() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"abc{}def">(p, "xyz");
  NTH_RAW_TEST_ASSERT(s == "abcxyzdef");
}

void Number() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"abc{}def">(p, 12345);
  NTH_RAW_TEST_ASSERT(s == "abc12345def");
}

void NoArguments() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"abc">(p);
  NTH_RAW_TEST_ASSERT(s == "abc");
}

void MultipleArguments() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"{}abc{}{}">(p, 1, 2, 3);
  NTH_RAW_TEST_ASSERT(s == "1abc23");
}

void Unicode() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"{}años">(p, "cumple");
  NTH_RAW_TEST_ASSERT(s == "cumpleaños");
}

void NonDefaultFormatSpec() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"{x}">(p, 17);
  NTH_RAW_TEST_ASSERT(s == "11");
}

void Bool() {
  std::string s;
  nth::string_printer p(s);
  nth::interpolate<"{}">(p, true);
  NTH_RAW_TEST_ASSERT(s == "true");
  s.clear();

  nth::interpolate<"{b}">(p, true);
  NTH_RAW_TEST_ASSERT(s == "true");
  s.clear();

  nth::interpolate<"{d}">(p, true);
  NTH_RAW_TEST_ASSERT(s == "1");
  s.clear();

  nth::interpolate<"{B}">(p, true);
  NTH_RAW_TEST_ASSERT(s == "True");
  s.clear();

  nth::interpolate<"{B!}">(p, true);
  NTH_RAW_TEST_ASSERT(s == "TRUE");
  s.clear();
}

struct point {
  friend constexpr nth::io::interpolation_spec NthDefaultFormatSpec(
      decltype(nth::type<point>)) {
    return nth::io::interpolation_spec::from<"({}, {})">();
  }

  friend nth::io::interpolation_spec NthFormatSpec(
      nth::interpolation_string_view s, decltype(nth::type<point>)) {
    return nth::io::interpolation_spec(s);
  }

  template <nth::io::printer_type P>
  friend void NthFormat(P p, nth::io::format_spec<point> spec,
                        point const &pt) {
    nth::interpolate(p, spec, pt.x, pt.y);
  }
  int x = 10;
  int y = 20;
};

void UserDefined() {
  std::string s;
  nth::string_printer p(s);

  point pt;
  nth::interpolate<"{}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 20)");
  s.clear();

  nth::interpolate<"{({}, {})}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 20)");
  s.clear();

  nth::interpolate<"{({}, {x})}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 14)");
  s.clear();

  nth::interpolate<"{({x}, {})}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "(a, 20)");
  s.clear();
}

}  // namespace

int main() {
  Simple();
  Number();
  NoArguments();
  MultipleArguments();
  Unicode();
  NonDefaultFormatSpec();
  Bool();
  UserDefined();
}
