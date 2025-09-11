#include "nth/format/format.h"

#include <string>
#include <string_view>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "nth/test/test.h"

namespace {

struct S {
  friend void NthFormat(nth::io::writer auto& w, auto&, S const& s) {
    nth::io::write_text(w, "[[");
    nth::io::write_text(w, s.str);
    nth::io::write_text(w, "]]");
  }
  std::string str;
};

NTH_TEST("format/container") {
  std::string result;
  nth::io::string_writer w(result);
  std::vector<S> v;
  nth::container_formatter<nth::default_formatter_t> fmt("{", ", ", "}", {});

  nth::format(w, fmt, v);
  NTH_ASSERT(result == "{}");

  result.clear();
  v.push_back({.str = "hello"});
  nth::format(w, fmt, v);
  NTH_ASSERT(result == "{[[hello]]}");

  result.clear();
  v.push_back({.str = "world"});
  nth::format(w, fmt, v);
  NTH_ASSERT(result == "{[[hello]], [[world]]}");
  NTH_ASSERT(nth::format_to_string(fmt, v) == "{[[hello]], [[world]]}");
}

struct StringRef {
  operator std::string_view() const { return content; }
  std::string_view content;
};

NTH_TEST("format/debug/container") {
  std::vector<std::string> v{"hello", "world"};
  std::string result;
  nth::io::string_writer w(result);
  nth::format(w, nth::debug_formatter, v);
  NTH_EXPECT(result == "[\"hello\", \"world\"]");

  result.clear();
  std::vector<StringRef> v2{{.content = "hello"}, {.content = "world"}};
  nth::format(w, nth::debug_formatter, v);
  NTH_EXPECT(result == "[\"hello\", \"world\"]");
  NTH_EXPECT(nth::format_to_string(nth::debug_formatter, v) ==
             "[\"hello\", \"world\"]");
}

static_assert(
    nth::type<decltype(NthDefaultFormatter(nth::type<std::string>))> ==
    nth::type<nth::text_formatter>);
static_assert(
    nth::type<decltype(NthDefaultFormatter(nth::type<std::string_view>))> ==
    nth::type<nth::text_formatter>);
static_assert(nth::type<decltype(NthDefaultFormatter(nth::type<StringRef>))> ==
              nth::type<nth::text_formatter>);

}  // namespace
