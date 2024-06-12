#include "nth/strings/interpolate/interpolate.h"

#include "nth/io/string_printer.h"
#include "nth/meta/type.h"
#include "nth/test/raw/test.h"

namespace {

#if 0
TODO: Death tests are not yet supported.
NTH_TEST("interpolation_string/construction") {
  NTH_EXPECT([] { interpolation_string("{"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{x}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{{}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{}{x"); } >>= nth::AbortsExecution());
  [[maybe_unused]] constexpr interpolation_string f1("{}");
  [[maybe_unused]] constexpr interpolation_string f2("{}{}");
}

NTH_TEST("interpolation_string/utf8-construction") {
  [[maybe_unused]] constexpr interpolation_string f1("\xc0\x80");
  [[maybe_unused]] constexpr interpolation_string f2("\xc0\x80{}");
  NTH_EXPECT([] { (void)interpolation_string("\xc0\x80}"); } >>=
             nth::AbortsExecution());
}
#endif

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
  friend constexpr auto NthDefaultFormatSpec(decltype(nth::type<point>)) {
    struct {
      nth::io::format_spec<int> specs[2] = {
          nth::io::default_format_spec<int>(),
          nth::io::default_format_spec<int>()};
    } s;
    return s;
  }

  template <nth::interpolation_string S>
  friend consteval auto NthFormatSpec(decltype(nth::type<point>)) {
    if constexpr (S == "pair") {
      using T = decltype(NthDefaultFormatSpec(nth::type<point>));
      return T{
        .specs = {
          nth::io::default_format_spec<int>(),
          nth::io::default_format_spec<int>(),
        }
      };
    } else {
      return nth::io::default_format_spec<point>();
    }
  }

  template <nth::io::printer_type P>
  friend void NthFormat(P p, auto spec, point const &pt) {
    P::print(p, spec.specs[0], pt.x);
    P::print(p, spec.specs[1], pt.y);
  }
  int x = 1;
  int y = 2;
};

void UserDefined() {
  std::string s;
  nth::string_printer p(s);

  point pt;
  nth::interpolate<"{}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "12");
  s.clear();

  nth::interpolate<"{pair}">(p, pt);
  NTH_RAW_TEST_ASSERT(s == "12");
  s.clear();
}

}  // namespace

int main() {
  Simple();
  Number();
  NoArguments();
  MultipleArguments();
  NoArguments();
  Unicode();
  NonDefaultFormatSpec();
  Bool();
  UserDefined();
}
