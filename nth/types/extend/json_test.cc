#include "nth/types/extend/json.h"

#include <string>
#include <vector>

#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {

struct Empty : nth::extend<Empty>::with<nth::ext::format_json> {};
struct OneField : nth::extend<OneField>::with<nth::ext::format_json> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::ext::format_json> {
  int n;
  std::string s;
  std::vector<std::string> v;
};

NTH_TEST("extend/json") {
  std::string s;
  nth::io::string_writer w(s);

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{}, Empty{});
  NTH_EXPECT(s == "{}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{}, OneField{});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 3\n"
             "}");

  s.clear();
  nth::io::format(w, nth::ext::json_formatter{},
                  ManyFields{.n = 3, .s = "hello", .v = {"abc", "\"quoted\""}});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 3,\n"
             "  \"s\": \"hello\",\n"
             "  \"v\": [\n"
             "    \"abc\",\n"
             "    \"\\\"quoted\\\"\"\n"
             "  ]\n"
             "}");
}

}  // namespace
