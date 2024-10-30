#include "nth/format/interpolate.h"

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

void Pointer() {
  std::string s;
  uintptr_t ptr_value = 0x12345678;
  nth::io::string_writer w(s);
  nth::interpolate<"{}">(w, reinterpret_cast<void *>(ptr_value));
  if constexpr (sizeof(uintptr_t) == 8) {
    NTH_RAW_TEST_ASSERT(s == "0x0000000012345678");
  } else {
    NTH_RAW_TEST_ASSERT(s == "0x12345678");
  }
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
  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<point>) {
    if constexpr (S.empty()) {
      return nth::trivial_formatter{};
    } else {
      return nth::interpolating_formatter<S>{};
    }
  }

  template <nth::interpolation_string S>
  friend void NthFormat(nth::io::writer auto &w,
                        nth::interpolating_formatter<S> &, point const &pt) {
    nth::interpolate<S>(w, pt.x, pt.y);
  }

  friend void NthFormat(nth::io::writer auto &w, auto &, point const &pt) {
    nth::interpolate<"({}, {})">(w, pt.x, pt.y);
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
  Pointer();
  NoArguments();
  MultipleArguments();
  Unicode();
  NonDefaultFormatSpec();
  Bool();
  UserDefined();
}
