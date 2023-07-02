#include "nth/strings/format/format.h"

#include "gtest/gtest.h"
#include "nth/io/string_printer.h"
#include "nth/strings/format/universal.h"

namespace nth {
namespace {

struct NotAFormatter {};

struct AFormatter {
  void operator()(Printer auto&, char) const;
};

TEST(Formatter, DistinguishesFormattersCorrectly) {
  {
    constexpr bool b = FormatterFor<NotAFormatter, bool>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = FormatterFor<NotAFormatter, char>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = FormatterFor<NotAFormatter, bool, char>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = FormatterFor<NotAFormatter>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<AFormatter, bool>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<AFormatter, char>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<AFormatter, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<AFormatter, int, char>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<AFormatter>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<bool>, bool>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<char>, char>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<int>, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<int>, int, char>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<int, char>, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<char, int>, int, char>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = FormatterFor<MinimalFormatterFor<>>;
    EXPECT_TRUE(b);
  }
}

TEST(FormatString, Construction) {
  EXPECT_DEATH({ FormatString("{"); }, "");
  EXPECT_DEATH({ FormatString("}"); }, "");
  EXPECT_DEATH({ FormatString("{x}"); }, "");
  EXPECT_DEATH({ FormatString("{{}"); }, "");
  EXPECT_DEATH({ FormatString("{}{x"); }, "");
  [[maybe_unused]] constexpr FormatString f1("{}");
  [[maybe_unused]] constexpr FormatString f2("{}{}");
}

TEST(FormatString, UTf8Construction) {
  [[maybe_unused]] constexpr FormatString f1("\xc0\x80");
  [[maybe_unused]] constexpr FormatString f2("\xc0\x80{}");
  EXPECT_DEATH({ FormatString("\xc0\x80}"); }, "");
}


struct TrivialFormatter {
  void operator()(nth::Printer auto& p, std::string_view s) const {
    p.write(s);
  }
};

TEST(FormatString, Printer) {
  std::string s;
  StringPrinter p(s);
  TrivialFormatter t;
  Format<"abc{}def">(p, t, "xyz");
  EXPECT_EQ(s, "abcxyzdef");
  s.clear();

  Format<"abc{}def">(p, t, "12345");
  EXPECT_EQ(s, "abc12345def");
  s.clear();

  Format<"abc">(p, t);
  EXPECT_EQ(s, "abc");
  s.clear();

  Format<"{}abc{}{}">(p, t, "1", "2", "3");
  EXPECT_EQ(s, "1abc23");
  s.clear();

  Format<"{}años">(p, t, "cumple");
  EXPECT_EQ(s, "cumpleaños");
}

TEST(FormatString, UniversalFormatter) {
  std::string s;
  StringPrinter p(s);
  universal_formatter f({
      .depth    = 3,
      .fallback = "...",
  });
  Format<"abc{}def">(p, f, false);
  EXPECT_EQ(s, "abcfalsedef");
  s.clear();

  Format<"abc{}def">(p, f, true);
  EXPECT_EQ(s, "abctruedef");
  s.clear();

  Format<"abc">(p, f);
  EXPECT_EQ(s, "abc");
  s.clear();

  Format<"{}abc{}{}">(p, f, 1, 2, 3);
  EXPECT_EQ(s, "1abc23");
  s.clear();

  Format<"{}años">(p, f, "cumple");
  EXPECT_EQ(s, "cumpleaños");
}

}  // namespace
}  // namespace nth
