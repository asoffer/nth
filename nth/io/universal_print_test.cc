#include "nth/io/universal_print.h"

#include <string_view>

#include "gtest/gtest.h"
#include "nth/io/string_printer.h"

namespace nth {
namespace {

TEST(UniversalPrint, Builtin) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, "hello");
  UniversalPrint(p, '!');
  EXPECT_EQ(s, "hello!");
}

TEST(UniversalPrint, Ostream) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, 1234);
  EXPECT_EQ(s, "1234");
}

TEST(UniversalPrint, Bools) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, true);
  EXPECT_EQ(s, "true");
  s.clear();
  UniversalPrint(p, false);
  EXPECT_EQ(s, "false");
}

struct S {
  int32_t value;
};

TEST(UniversalPrint, Tuple) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, std::tuple<>{});
  EXPECT_EQ(s, "{}");
  s.clear();

  UniversalPrint(p, std::tuple<int>{});
  EXPECT_EQ(s, "{0}");
  s.clear();

  UniversalPrint(p, std::tuple<int, bool>{});
  EXPECT_EQ(s, "{0, false}");
  s.clear();
}

TEST(UniversalPrint, Optional) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, std::nullopt);
  EXPECT_EQ(s, "std::nullopt");
  s.clear();
  UniversalPrint(p, std::optional<int>());
  EXPECT_EQ(s, "std::nullopt");
  s.clear();
  UniversalPrint(p, std::optional<int>(3));
  EXPECT_EQ(s, "3");
}

TEST(UniversalPrint, Variant) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, std::variant<int, bool>(5));
  EXPECT_EQ(s, "5");
  s.clear();
  UniversalPrint(p, std::variant<int, bool>(true));
  EXPECT_EQ(s, "true");
}

TEST(UniversalPrint, Fallback) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, S{.value = 17});
  EXPECT_EQ(
      s,
      "[Unprintable value of type nth::(anonymous namespace)::S: 11 00 00 00]");
}

TEST(UniversalPrint, Depth) {
  std::string s;
  StringPrinter p(s);
  UniversalPrint(p, std::optional<std::optional<std::optional<int>>>(5));
  EXPECT_EQ(s, "5");

  s.clear();
  UniversalPrint(p, std::optional<int>(5), {.depth = 1, .fallback = "[...]"});
  EXPECT_EQ(s, "5");

  s.clear();
  UniversalPrint(p, std::optional<std::optional<int>>(5),
                 {.depth = 1, .fallback = "[...]"});
  EXPECT_EQ(s, "[...]");

  s.clear();
  UniversalPrint(p, std::optional<std::optional<std::optional<int>>>(5),
                 {.depth = 1, .fallback = "[...]"});
  EXPECT_EQ(s, "[...]");
}

}  // namespace
}  // namespace nth
