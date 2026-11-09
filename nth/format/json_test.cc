#include "nth/format/json.h"

#include <map>
#include <string>
#include <vector>

#include "nth/format/structure.h"
#include "nth/test/test.h"
#include "nth/types/structure.h"

namespace {

template <typename T>
std::string json(T const &obj) {
  std::string s;
  nth::io::string_writer w(s);
  nth::format(w, nth::json_formatter{}, obj);
  return s;
}

NTH_TEST("format/json/bool") {
  NTH_EXPECT(json(true) == "true");
  NTH_EXPECT(json(false) == "false");
}

NTH_TEST("format/json/number") {
  NTH_EXPECT(json(1) == "1");
  NTH_EXPECT(json(-1) == "-1");
  NTH_EXPECT(json(17u) == "17");
  NTH_EXPECT(json(3.14) == "3.14");
}

NTH_TEST("format/json/string") {
  NTH_EXPECT(json("") == R"("")");
  NTH_EXPECT(json(std::string_view("")) == R"("")");
  NTH_EXPECT(json(std::string("")) == R"("")");

  NTH_EXPECT(json("abc") == R"("abc")");
  NTH_EXPECT(json(std::string_view("abc")) == R"("abc")");
  NTH_EXPECT(json(std::string("abc")) == R"("abc")");

  NTH_EXPECT(json("\"hello\n\"") == R"("\"hello\n\"")");
  NTH_EXPECT(json(std::string_view("\"hello\n\"")) == R"("\"hello\n\"")");
  NTH_EXPECT(json(std::string("\"hello\n\"")) == R"("\"hello\n\"")");
  NTH_EXPECT(json(std::string("hello\\world")) == R"("hello\\world")");
  NTH_EXPECT(json(std::string("hello\0world", 11)) ==
             std::string_view(R"("hello\u0000world")"));
  NTH_EXPECT(json(std::string("hello\u0000world", 11)) ==
             std::string_view(R"("hello\u0000world")"));
  NTH_EXPECT(json(std::string("hello\7world")) ==
             std::string_view(R"("hello\u0007world")"));
  NTH_EXPECT(json(std::string("hello\\0world")) ==
             std::string_view(R"("hello\\0world")"));
  NTH_EXPECT(json(std::string("hello\tworld")) ==
             std::string_view(R"("hello\tworld")"));
}

NTH_TEST("format/json/string/utf8") {
  // Well-formed UTF-8 is written unchanged.
  NTH_EXPECT(json("caf\xc3\xa9") == "\"caf\xc3\xa9\"");
  NTH_EXPECT(json("\xe2\x82\xac" "5") == "\"\xe2\x82\xac" "5\"");
  NTH_EXPECT(json("\xf0\x9f\x98\x80") == "\"\xf0\x9f\x98\x80\"");
  NTH_EXPECT(json("\xf4\x8f\xbf\xbf") == "\"\xf4\x8f\xbf\xbf\"");
  NTH_EXPECT(json("\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80") ==
             "\"\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80\"");

  // Escapes still apply around UTF-8.
  NTH_EXPECT(json("\"\xc3\xa9\"\n") == "\"\\\"\xc3\xa9\\\"\\n\"");
  NTH_EXPECT(json("\xc3\xa9\\\t\xc3\xa9") == "\"\xc3\xa9\\\\\\t\xc3\xa9\"");
  NTH_EXPECT(json(std::string("\xc3\xa9\0", 3)) == "\"\xc3\xa9\\u0000\"");
}

NTH_TEST("format/json/string/ill-formed-utf8") {
  // Each byte of an ill-formed sequence is escaped.
  NTH_EXPECT(json("\x80") == R"("\u0080")");
  NTH_EXPECT(json("\xff") == R"("\u00ff")");
  NTH_EXPECT(json("\xc0\x80") == R"("\u00c0\u0080")");
  NTH_EXPECT(json("\xed\xa0\x80") == R"("\u00ed\u00a0\u0080")");
  NTH_EXPECT(json("\xf4\x90\x80\x80") == R"("\u00f4\u0090\u0080\u0080")");
  NTH_EXPECT(json("\x7f") == R"("\u007f")");

  // A truncated sequence at the end of the string.
  NTH_EXPECT(json("abc\xc3") == R"("abc\u00c3")");
  NTH_EXPECT(json("abc\xe2\x82") == R"("abc\u00e2\u0082")");

  // A truncated sequence interrupted by ASCII, including characters that must
  // themselves be escaped.
  NTH_EXPECT(json("\xc3" "a") == R"("\u00c3a")");
  NTH_EXPECT(json("\xe2\x82\n") == R"("\u00e2\u0082\n")");
  NTH_EXPECT(json("\xc3\"") == R"("\u00c3\"")");

  // A stray continuation byte after a well-formed sequence is escaped, while
  // the sequence itself is kept.
  NTH_EXPECT(json("\xc3\xa9\xa9") == "\"\xc3\xa9\\u00a9\"");
  NTH_EXPECT(json("\xc3\xa9\x80\xc3\xa9") == "\"\xc3\xa9\\u0080\xc3\xa9\"");

  // A well-formed sequence directly after an ill-formed one is kept.
  NTH_EXPECT(json("\xc3\xc3\xa9") == "\"\\u00c3\xc3\xa9\"");
}

NTH_TEST("format/json/array") {
  NTH_EXPECT(json(std::array<int, 0>{}) == "[]");
  NTH_EXPECT(json(std::vector<double>{}) == "[]");

  NTH_EXPECT(json(std::array{1}) ==
             "[\n"
             "  1\n"
             "]");
  NTH_EXPECT(json(std::vector{3.14}) ==
             "[\n"
             "  3.14\n"
             "]");

  NTH_EXPECT(json(std::array{1, 4, 9}) ==
             "[\n"
             "  1,\n"
             "  4,\n"
             "  9\n"
             "]");
  NTH_EXPECT(json(std::vector{3.14, 2.72, 1.62}) ==
             "[\n"
             "  3.14,\n"
             "  2.72,\n"
             "  1.62\n"
             "]");
  NTH_EXPECT(
      json(std::array<std::vector<int>, 4>{{{1, 2, 3}, {4}, {}, {5, 6}}}) ==
      "[\n"
      "  [\n"
      "    1,\n"
      "    2,\n"
      "    3\n"
      "  ],\n"
      "  [\n"
      "    4\n"
      "  ],\n"
      "  [],\n"
      "  [\n"
      "    5,\n"
      "    6\n"
      "  ]\n"
      "]");
}

NTH_TEST("format/json/associative") {
  NTH_EXPECT(json(std::map<int, std::string>{}) == "{}");

  NTH_EXPECT(json(std::map<int, std::string>{{1, ""}}) ==
             "{\n"
             "  1: \"\"\n"
             "}");
  NTH_EXPECT(json(std::map<int, std::string>{{1, ""}, {2, "\"hello\""}}) ==
             "{\n"
             "  1: \"\",\n"
             "  2: \"\\\"hello\\\"\"\n"
             "}");
}

struct Object {
  int n;
  std::string s;

  friend void NthFormat(nth::io::writer auto &w, nth::json_formatter &f,
                        Object const &obj) {
    nth::begin_format<nth::structure::associative>(w, f);
    nth::begin_format<nth::structure::key>(w, f);
    nth::format(w, f, "n");
    nth::end_format<nth::structure::key>(w, f);
    nth::begin_format<nth::structure::value>(w, f);
    nth::format(w, f, obj.n);
    nth::end_format<nth::structure::value>(w, f);
    nth::begin_format<nth::structure::key>(w, f);
    nth::format(w, f, "s");
    nth::end_format<nth::structure::key>(w, f);
    nth::begin_format<nth::structure::value>(w, f);
    nth::format(w, f, obj.s);
    nth::end_format<nth::structure::value>(w, f);
    nth::end_format<nth::structure::associative>(w, f);
  }
};
NTH_TEST("format/json/object") {
  NTH_EXPECT(json(Object{.n = 3, .s = "hi"}) ==
             "{\n"
             "  \"n\": 3,\n"
             "  \"s\": \"hi\"\n"
             "}");
}

}  // namespace
