#include "nth/strings/interpolate.h"

#include "gtest/gtest.h"
#include "nth/io/string_printer.h"
#include "nth/strings/format/formatter.h"
#include "nth/strings/format/universal.h"

namespace nth {
namespace {

TEST(InterpolationString, Construction) {
  EXPECT_DEATH({ InterpolationString("{"); }, "");
  EXPECT_DEATH({ InterpolationString("}"); }, "");
  EXPECT_DEATH({ InterpolationString("{x}"); }, "");
  EXPECT_DEATH({ InterpolationString("{{}"); }, "");
  EXPECT_DEATH({ InterpolationString("{}{x"); }, "");
  [[maybe_unused]] constexpr InterpolationString f1("{}");
  [[maybe_unused]] constexpr InterpolationString f2("{}{}");
}

TEST(InterpolationString, UTf8Construction) {
  [[maybe_unused]] constexpr InterpolationString f1("\xc0\x80");
  [[maybe_unused]] constexpr InterpolationString f2("\xc0\x80{}");
  EXPECT_DEATH({ InterpolationString("\xc0\x80}"); }, "");
}

struct TrivialFormatter {
  void operator()(nth::Printer auto& p, std::string_view s) const {
    p.write(s);
  }
};

TEST(InterpolationString, Printer) {
  std::string s;
  string_printer p(s);
  TrivialFormatter t;
  Interpolate<"abc{}def">(p, t, "xyz");
  EXPECT_EQ(s, "abcxyzdef");
  s.clear();

  Interpolate<"abc{}def">(p, t, "12345");
  EXPECT_EQ(s, "abc12345def");
  s.clear();

  Interpolate<"abc">(p, t);
  EXPECT_EQ(s, "abc");
  s.clear();

  Interpolate<"{}abc{}{}">(p, t, "1", "2", "3");
  EXPECT_EQ(s, "1abc23");
  s.clear();

  Interpolate<"{}años">(p, t, "cumple");
  EXPECT_EQ(s, "cumpleaños");
}

TEST(InterpolationString, UniversalFormatter) {
  std::string s;
  string_printer p(s);
  universal_formatter f({
      .depth    = 3,
      .fallback = "...",
  });
  Interpolate<"abc{}def">(p, f, false);
  EXPECT_EQ(s, "abcfalsedef");
  s.clear();

  Interpolate<"abc{}def">(p, f, true);
  EXPECT_EQ(s, "abctruedef");
  s.clear();

  Interpolate<"abc">(p, f);
  EXPECT_EQ(s, "abc");
  s.clear();

  Interpolate<"{}abc{}{}">(p, f, 1, 2, 3);
  EXPECT_EQ(s, "1abc23");
  s.clear();

  Interpolate<"{}años">(p, f, "cumple");
  EXPECT_EQ(s, "cumpleaños");
}

}  // namespace
}  // namespace nth
