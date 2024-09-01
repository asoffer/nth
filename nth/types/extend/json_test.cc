#include "nth/types/extend/json.h"

#include <map>
#include <string>
#include <vector>

#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {

struct Empty : nth::extend<Empty>::with<nth::ext::format_json> {};
struct Scalar : nth::extend<Scalar>::with<nth::ext::format_json> {
  int n = 3;
};

struct Array : nth::extend<Array>::with<nth::ext::format_json> {
  std::vector<std::string> v;
};

struct Associative : nth::extend<Associative>::with<nth::ext::format_json> {
  std::map<std::string, std::vector<std::string>> m;
};

NTH_TEST("extend/json/empty") {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::format(w, nth::ext::json_formatter{}, Empty{});
  NTH_EXPECT(s == "{}");
}

NTH_TEST("extend/json/one-scalar") {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::format(w, nth::ext::json_formatter{}, Scalar{});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 3\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{}, Scalar{.n = 17});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 17\n"
             "}");
}

NTH_TEST("extend/json/one-array") {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::format(w, nth::ext::json_formatter{}, Array{});
  NTH_EXPECT(s ==
             "{\n"
             "  \"v\": []\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{}, Array{.v = {"abc"}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"v\": [\n"
             "    \"abc\"\n"
             "  ]\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{},
                  Array{.v = {"abc", "\"quoted\""}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"v\": [\n"
             "    \"abc\",\n"
             "    \"\\\"quoted\\\"\"\n"
             "  ]\n"
             "}");
}

NTH_TEST("extend/json/one-associative") {
  std::string s;
  nth::io::string_writer w(s);
  nth::io::format(w, nth::ext::json_formatter{}, Associative{});
  NTH_EXPECT(s ==
             "{\n"
             "  \"m\": {}\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{}, Associative{.m = {{"", {}}}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"m\": {\n"
             "    \"\": []\n"
             "  }\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{},
                  Associative{.m = {{"abc", {"def", "ghi"}}}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"m\": {\n"
             "    \"abc\": [\n"
             "      \"def\",\n"
             "      \"ghi\"\n"
             "    ]\n"
             "  }\n"
             "}");
  s.clear();
  nth::io::format(w, nth::ext::json_formatter{},
                  Associative{.m = {{"abc", {"def", "ghi"}}, {"\"quoted\"", {}}}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"m\": {\n"
             "    \"\\\"quoted\\\"\": [],\n"
             "    \"abc\": [\n"
             "      \"def\",\n"
             "      \"ghi\"\n"
             "    ]\n"
             "  }\n"
             "}");
}

}  // namespace
