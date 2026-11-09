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

NTH_TEST("format/pointer") {
  uintptr_t ptr_value = 0x12345678;
  NTH_LOG("{}")<<=nth::format_to_string(nth::pointer_formatter{},
                                   reinterpret_cast<void*>(ptr_value));
  NTH_EXPECT(nth::format_to_string(nth::pointer_formatter{},
                                   reinterpret_cast<void*>(ptr_value)) ==
             "0x12345678");
  ptr_value = 0;
  NTH_EXPECT(nth::format_to_string(nth::pointer_formatter{},
                                   reinterpret_cast<void*>(ptr_value)) ==
             "0x0");
}
NTH_TEST("format/variant") {
  std::variant<int, bool> v = 3;
  NTH_ASSERT(nth::format_to_string(v) == "3");
  v = true;
  NTH_EXPECT(nth::format_to_string(v) == "true");
}

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

NTH_TEST("format/utf8/sequence-length/not-utf8") {
  NTH_EXPECT(nth::Utf8SequenceLength("") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("a") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength(std::string_view("\0", 1)) == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\x7f") == 0u);
}

NTH_TEST("format/utf8/sequence-length/well-formed") {
  // Two bytes: U+0080, U+00E9, U+07FF.
  NTH_EXPECT(nth::Utf8SequenceLength("\xc2\x80") == 2u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xc3\xa9") == 2u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xdf\xbf") == 2u);

  // Three bytes: U+0800, U+20AC, U+D7FF, U+E000, U+FFFF.
  NTH_EXPECT(nth::Utf8SequenceLength("\xe0\xa0\x80") == 3u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xe2\x82\xac") == 3u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xed\x9f\xbf") == 3u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xee\x80\x80") == 3u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xef\xbf\xbf") == 3u);

  // Four bytes: U+10000, U+1F600, U+10FFFF.
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x90\x80\x80") == 4u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x9f\x98\x80") == 4u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf4\x8f\xbf\xbf") == 4u);

  // Only the leading sequence is measured.
  NTH_EXPECT(nth::Utf8SequenceLength("\xc3\xa9" "abc") == 2u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xe2\x82\xac\xe2\x82\xac") == 3u);
}

NTH_TEST("format/utf8/sequence-length/ill-formed") {
  // Continuation bytes cannot lead.
  NTH_EXPECT(nth::Utf8SequenceLength("\x80") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xbf") == 0u);

  // Bytes that never appear in UTF-8.
  NTH_EXPECT(nth::Utf8SequenceLength("\xc0\x80") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xc1\xbf") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf5\x80\x80\x80") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xff") == 0u);

  // Overlong encodings.
  NTH_EXPECT(nth::Utf8SequenceLength("\xe0\x9f\xbf") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x8f\xbf\xbf") == 0u);

  // Surrogates: U+D800, U+DFFF.
  NTH_EXPECT(nth::Utf8SequenceLength("\xed\xa0\x80") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xed\xbf\xbf") == 0u);

  // Past U+10FFFF.
  NTH_EXPECT(nth::Utf8SequenceLength("\xf4\x90\x80\x80") == 0u);

  // Truncated.
  NTH_EXPECT(nth::Utf8SequenceLength("\xc3") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xe2\x82") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x9f\x98") == 0u);

  // A byte that should continue the sequence does not.
  NTH_EXPECT(nth::Utf8SequenceLength("\xc3" "a") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xe2" "a\xac") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xe2\x82" "a") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x9f\x98" "a") == 0u);
  NTH_EXPECT(nth::Utf8SequenceLength("\xf0\x9f\xc0\x80") == 0u);
}

static_assert(nth::Utf8SequenceLength("\xf0\x9f\x98\x80") == 4u);
static_assert(nth::Utf8SequenceLength("\xed\xa0\x80") == 0u);

}  // namespace
