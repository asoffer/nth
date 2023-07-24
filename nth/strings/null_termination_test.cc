#include "nth/strings/null_termination.h"

#include "nth/test/test.h"

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

NTH_TEST("InvokeOnNullTerminated/string-reuses-data") {
  std::string s =
      "this is a string so long that it definitely cannot fit in the "
      "small-string-optimization buffer.";
  char const * p  = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s);
  NTH_EXPECT(p == s.c_str());
}

NTH_TEST("InvokeOnNullTerminated/arrays-are-checked-for-null-termination") {
  char s1[] = "some-string";
  char const *p = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s1);
  NTH_EXPECT(p == s1);

  char s2[3] ={'a', 'b', 'c'};
  std::string str;
  InvokeOnNullTerminated([&](char const *s) { str = p = s; }, s2);
  NTH_EXPECT(p != s2);
  NTH_EXPECT(str == "abc");
}

NTH_TEST("InvokeOnNullTerminated/string-views") {
  std::string_view s = "some-string";
  char const *p = nullptr;
  std::string str;
  InvokeOnNullTerminated([&](char const *s) { str = p = s; },s);
  NTH_EXPECT(p != s.data());
  NTH_EXPECT(str == "some-string");
}

NTH_TEST("InvokeOnNullTerminated/char-ptrs-are-assumed-to-be-null-terminated") {
  char s[] = "some-string";
  char const *p = nullptr;
  InvokeOnNullTerminated([&](char const *s) { p = s; }, s);
  NTH_EXPECT(p == s);
}

}  // namespace
}  // namespace nth
