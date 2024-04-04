#include "nth/meta/compile_time_string.h"

#include <string_view>

namespace nth {

static_assert([] {
  constexpr compile_time_string s("abc");
  static_assert(s.size() == 3);
  static_assert(s.length() == 3);
  static_assert(s == std::string_view("abc"));
  return true;
}());

template <compile_time_string S>
struct Wrapper {
  static constexpr std::string_view view = S;
};

static_assert(Wrapper<"abc">::view == "abc");

static_assert([] {
  constexpr compile_time_string s1("abcdef");
  constexpr compile_time_string s2 = s1.substr<2, 3>();
  static_assert(s2 == std::string_view("cde"));
  constexpr compile_time_string s3 = s1.substr<3>();
  static_assert(s3 == std::string_view("def"));
  return true;
}());

static_assert([] {
  constexpr compile_time_string s("hello, hello");
  constexpr compile_time_string a = s.substr<0, 5>();
  constexpr compile_time_string b = s.substr<7>();
  constexpr compile_time_string c = s.substr<8>();
  static_assert(a == b);
  static_assert(a != c);
  return true;
}());

static_assert([] {
  constexpr compile_time_string a("hello, ");
  constexpr compile_time_string b("world!");
  static_assert(a + b == compile_time_string("hello, world!"));
  static_assert(a + (a.substr<1, 3>()) == compile_time_string("hello, ell"));
  return true;
}());

static_assert([] {
  constexpr compile_time_string a("hello, ");
  constexpr compile_time_string b("");
  constexpr compile_time_string c("\0");
  static_assert(not a.empty());
  static_assert(b.empty());
  static_assert(not c.empty());
  return true;
}());

}  // namespace nth

int main() { return 0; }
