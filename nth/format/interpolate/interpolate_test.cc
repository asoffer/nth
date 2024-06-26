#include "nth/format/interpolate/interpolate.h"

#include "nth/io/writer/string.h"
#include "nth/meta/type.h"
#include "nth/test/raw/test.h"

namespace {

void Simple() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"abc{}def">(w, "xyz");
  NTH_RAW_TEST_ASSERT(s == "abcxyzdef");
}

void Number() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"abc{}def">(w, 12345);
  NTH_RAW_TEST_ASSERT(s == "abc12345def");
}

void NoArguments() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"abc">(w);
  NTH_RAW_TEST_ASSERT(s == "abc");
}

void MultipleArguments() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"{}abc{}{}">(w, 1, 2, 3);
  NTH_RAW_TEST_ASSERT(s == "1abc23");
}

void Unicode() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"{}años">(w, "cumple");
  NTH_RAW_TEST_ASSERT(s == "cumpleaños");
}

void NonDefaultFormatSpec() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"{x}">(w, 17);
  NTH_RAW_TEST_ASSERT(s == "11");
}

void Bool() {
  std::string s;
  nth::io::string_writer w(s);
  nth::interpolate<"{}">(w, true);
  NTH_RAW_TEST_ASSERT(s == "true");
  s.clear();

  nth::interpolate<"{b}">(w, true);
  NTH_RAW_TEST_ASSERT(s == "true");
  s.clear();

  nth::interpolate<"{d}">(w, true);
  NTH_RAW_TEST_ASSERT(s == "1");
  s.clear();

  nth::interpolate<"{B}">(w, true);
  NTH_RAW_TEST_ASSERT(s == "True");
  s.clear();

  nth::interpolate<"{B!}">(w, true);
  NTH_RAW_TEST_ASSERT(s == "TRUE");
  s.clear();
}

struct point {
  using nth_format_spec = nth::interpolation_spec;

  friend constexpr nth::interpolation_spec NthDefaultFormatSpec(
      nth::type_tag<point>) {
    return nth::interpolation_spec::from<"({}, {})">();
  }

  friend nth::interpolation_spec NthFormatSpec(nth::interpolation_string_view s,
                                               nth::type_tag<point>) {
    return nth::interpolation_spec(s);
  }

  template <nth::io::forward_writer W>
  friend void NthFormat(W w, nth::format_spec<point> spec, point const &pt) {
    nth::interpolate(w, spec, pt.x, pt.y);
  }
  int x = 10;
  int y = 20;
};

void UserDefined() {
  std::string s;
  nth::io::string_writer w(s);

  point pt;
  nth::interpolate<"{}">(w, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 20)");
  s.clear();

  nth::interpolate<"{({}, {})}">(w, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 20)");
  s.clear();

  nth::interpolate<"{({}, {x})}">(w, pt);
  NTH_RAW_TEST_ASSERT(s == "(10, 14)");
  s.clear();

  nth::interpolate<"{({x}, {})}">(w, pt);
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
