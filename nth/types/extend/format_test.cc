#include "nth/types/extend/format.h"

#include <string>

#include "nth/test/test.h"
#include "nth/types/extend/extend.h"

namespace {

struct Empty : nth::extend<Empty>::with<nth::formatting> {};
struct OneField : nth::extend<OneField>::with<nth::formatting> {
  int n = 3;
};

struct ManyFields : nth::extend<ManyFields>::with<nth::formatting> {
  int n;
  std::string s;
};

NTH_TEST("extend/format") {
  std::string s;
  nth::io::string_writer w(s);

  s.clear();
  nth::io::format(w, Empty{});
  NTH_EXPECT(s == "{}");

  s.clear();
  nth::io::format(w, OneField{});
  NTH_EXPECT(s ==
             "{\n"
             "  .n = 3,\n"
             "}");

  s.clear();
  nth::io::format(w, ManyFields{.n = 3, .s = "hello"});
  NTH_EXPECT(s ==
             "{\n"
             "  .n = 3,\n"
             "  .s = hello,\n"
             "}");
}

}  // namespace
