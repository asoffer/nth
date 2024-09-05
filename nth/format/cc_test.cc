#include "nth/format/cc.h"

#include <map>
#include <string>
#include <vector>

#include "nth/test/test.h"
#include "nth/types/structure.h"

namespace {

template <typename T>
std::string cc(T const& obj) {
  std::string s;
  nth::io::string_writer w(s);
  nth::format(w, nth::cc_formatter{}, obj);
  return s;
}

NTH_TEST("format/cc/bool") {
  NTH_EXPECT(cc(true) == "true");
  NTH_EXPECT(cc(false) == "false");
}

NTH_TEST("format/cc/number") {
  NTH_EXPECT(cc(1) == "1");
  NTH_EXPECT(cc(-1) == "-1");
  NTH_EXPECT(cc(17u) == "17");
  NTH_EXPECT(cc(3.14) == "3.14");
}

NTH_TEST("format/cc/string") {
  NTH_EXPECT(cc("") == R"("")");
  NTH_EXPECT(cc(std::string_view("")) == R"("")");
  NTH_EXPECT(cc(std::string("")) == R"("")");

  NTH_EXPECT(cc("abc") == R"("abc")");
  NTH_EXPECT(cc(std::string_view("abc")) == R"("abc")");
  NTH_EXPECT(cc(std::string("abc")) == R"("abc")");

  NTH_EXPECT(cc("\"hello\n\"") == R"("\"hello\n\"")");
  NTH_EXPECT(cc(std::string_view("\"hello\n\"")) == R"("\"hello\n\"")");
  NTH_EXPECT(cc(std::string("\"hello\n\"")) == R"("\"hello\n\"")");
}

NTH_TEST("format/cc/array") {
  NTH_EXPECT(cc(std::array<int, 0>{}) == "{}");
  NTH_EXPECT(cc(std::vector<double>{}) == "{}");

  NTH_EXPECT(cc(std::array{1}) ==
             "{\n"
             "  1\n"
             "}");
  NTH_EXPECT(cc(std::vector{3.14}) ==
             "{\n"
             "  3.14\n"
             "}");

  NTH_EXPECT(cc(std::array{1, 4, 9}) ==
             "{\n"
             "  1,\n"
             "  4,\n"
             "  9\n"
             "}");
  NTH_EXPECT(cc(std::vector{3.14, 2.72, 1.62}) ==
             "{\n"
             "  3.14,\n"
             "  2.72,\n"
             "  1.62\n"
             "}");
  NTH_EXPECT(
      cc(std::array<std::vector<int>, 4>{{{1, 2, 3}, {4}, {}, {5, 6}}}) ==
      "{\n"
      "  {\n"
      "    1,\n"
      "    2,\n"
      "    3\n"
      "  },\n"
      "  {\n"
      "    4\n"
      "  },\n"
      "  {},\n"
      "  {\n"
      "    5,\n"
      "    6\n"
      "  }\n"
      "}");
}

NTH_TEST("format/cc/associative") {
  NTH_EXPECT(cc(std::map<int, std::string>{}) == "{}");

  NTH_EXPECT(cc(std::map<int, std::string>{{1, ""}}) ==
             "{\n"
             "  {1, \"\"}\n"
             "}");
  NTH_EXPECT(cc(std::map<int, std::string>{{1, ""}, {2, "\"hello\""}}) ==
             "{\n"
             "  {1, \"\"},\n"
             "  {2, \"\\\"hello\\\"\"}\n"
             "}");
}

struct Object {
  int n;
  std::string s;

  friend void NthFormat(nth::io::writer auto& w, nth::cc_formatter& f,
                        Object const& obj) {
    nth::begin_format<nth::structure::object>(w, f);
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
    nth::end_format<nth::structure::object>(w, f);
  }
};
NTH_TEST("format/cc/object") {
  NTH_EXPECT(cc(Object{.n = 3, .s = "hi"}) ==
             "{\n"
             "  .n = 3,\n"
             "  .s = \"hi\"\n"
             "}");
}

}  // namespace
