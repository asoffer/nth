#include "nth/types/extend/format.h"

#include <string>

#include <iostream>

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
  nth::format(w, Empty{});
  std::cerr << "!" << s << "!\n";
  NTH_EXPECT(s == "{}");

  s.clear();
  nth::format(w, OneField{});
  NTH_EXPECT(s == "{.n = 3}");

  s.clear();
  nth::format(w, ManyFields{.n = 3, .s = "hello"});
  NTH_EXPECT(s == "{.n = 3, .s = hello}");
}

}  // namespace
