#include "nth/strings/null_termination.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

static_assert(not StringLike<int>);
static_assert(StringLike<char const *>);
static_assert(StringLike<char *>);
static_assert(StringLike<char const[5]>);
static_assert(StringLike<char[5]>);
static_assert(StringLike<std::string>);
static_assert(StringLike<std::string_view>);
static_assert(StringLike<std::string &>);
static_assert(StringLike<std::string const &>);

TEST(InvokeOnNullTerminated, StringReusesData) {
  std::string s =
      "this is a string so long that it definitely cannot fit in the "
      "small-string-optimization buffer.";
  char const * p  = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s);
  EXPECT_EQ(p, s.c_str());
}

TEST(InvokeOnNullTerminated, ArraysAreCheckedForNullTermination) {
  char s1[] = "some-string";
  char const *p = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s1);
  EXPECT_EQ(p, s1);

  char s2[3] ={'a', 'b', 'c'};
  std::string str;
  InvokeOnNullTerminated([&](char const *s) { str = p = s; }, s2);
  EXPECT_NE(p, s2);
  EXPECT_EQ(str, "abc");
}

TEST(InvokeOnNullTerminated, StringViews) {
  std::string_view s = "some-string";
  char const *p = nullptr;
  std::string str;
  InvokeOnNullTerminated([&](char const *s) { str = p = s; }, s);
  EXPECT_NE(p, s.data());
  EXPECT_EQ(str, "some-string");
}

TEST(InvokeOnNullTerminated, CharPtrsAreAssumedToBeNullTerminated) {
  char s[] = "some-string";
  char const *p = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s);
  EXPECT_EQ(p, s);
}

}  // namespace
}  // namespace nth
