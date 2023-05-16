#include "nth/meta/compile_time_string.h"

#include <string_view>

#include "gtest/gtest.h"

namespace nth {

TEST(CompileTimeString, Construction) {
  constexpr CompileTimeString s("abc");
  EXPECT_EQ(s.size(), 3);
  EXPECT_EQ(s.length(), 3);
  EXPECT_EQ(s, std::string_view("abc"));
}

template <CompileTimeString S>
struct Wrapper {
  static constexpr std::string_view view = S;
};

TEST(CompileTimeString, TemplateParameters) {
  EXPECT_EQ(Wrapper<"abc">::view, "abc");
}

TEST(CompileTimeString, Substring) {
  constexpr CompileTimeString s1("abcdef");
  constexpr CompileTimeString s2 = s1.substr<2, 3>();
  EXPECT_EQ(s2, std::string_view("cde"));
  constexpr CompileTimeString s3 = s1.substr<3>();
  EXPECT_EQ(s3, std::string_view("def"));
}

TEST(CompileTimeString, SubstringEquality) {
  constexpr CompileTimeString s("hello, hello");
  constexpr CompileTimeString a = s.substr<0, 5>();
  constexpr CompileTimeString b = s.substr<7>();
  constexpr CompileTimeString c = s.substr<8>();
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST(CompileTimeString, Concatenation) {
  constexpr CompileTimeString a("hello, ");
  constexpr CompileTimeString b("world!");
  EXPECT_EQ(a + b, CompileTimeString("hello, world!"));
  EXPECT_EQ(a + (a.substr<1, 3>()), CompileTimeString("hello, ell"));
}

TEST(CompileTimeString, Empty) {
  constexpr CompileTimeString a("hello, ");
  constexpr CompileTimeString b("");
  constexpr CompileTimeString c("\0");
  EXPECT_FALSE(a.empty());
  EXPECT_TRUE(b.empty());
  EXPECT_FALSE(c.empty());
}

}  // namespace nth
