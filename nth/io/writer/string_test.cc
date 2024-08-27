#include "nth/io/writer/string.h"

#include <string_view>

#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("string_writer/basic") {
  std::string s;
  string_writer w(s);
  NTH_EXPECT(s.empty());
}

NTH_TEST("string_writer/write") {
  std::string s;
  string_writer w(s);
  NTH_ASSERT(w.write(nth::byte_range(std::string_view("abcd"))).written() ==
             4u);
  NTH_EXPECT(s.size() == 4u);
  NTH_EXPECT(s == "abcd");
  NTH_ASSERT(w.write(nth::byte_range(std::string_view("efgh"))).written() ==
             4u);
  NTH_EXPECT(s == "abcdefgh");
}

}  // namespace
}  // namespace nth::io
