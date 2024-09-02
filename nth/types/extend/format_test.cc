#include "nth/types/extend/format.h"

#include <string>

#include "nth/io/format/json.h"
#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {

struct Empty : nth::extend<Empty>::with<nth::ext::format> {};
struct OneField : nth::extend<OneField>::with<nth::ext::format> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::ext::format> {
  int n;
  std::string s;
};

NTH_TEST("extend/format/cc-default") {
  std::string s;
  nth::io::string_writer w(s);

  s.clear();
  nth::io::format(w, Empty{});
  NTH_EXPECT(s == "{}");

  s.clear();
  nth::io::format(w, OneField{});
  NTH_EXPECT(s ==
             "{\n"
             "  .n = 3\n"
             "}");

  s.clear();
  nth::io::format(w, ManyFields{.n = 3, .s = "hello"});
  NTH_EXPECT(s ==
             "{\n"
             "  .n = 3,\n"
             "  .s = \"hello\"\n"
             "}");
}

NTH_TEST("extend/format/json") {
  std::string s;
  nth::io::string_writer w(s);

  s.clear();
  nth::io::format(w, nth::io::json_formatter{}, Empty{});
  NTH_EXPECT(s == "{}");

  s.clear();
  nth::io::format(w, nth::io::json_formatter{}, OneField{});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 3\n"
             "}");

  s.clear();
  nth::io::format(w, nth::io::json_formatter{},
                  ManyFields{.n = 3, .s = "hello"});
  NTH_EXPECT(s ==
             "{\n"
             "  \"n\": 3,\n"
             "  \"s\": \"hello\"\n"
             "}");
}

}  // namespace
