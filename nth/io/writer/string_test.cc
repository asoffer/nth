#include "nth/io/writer/string.h"

#include <string_view>

#include "nth/io/writer/writer.h"
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
  NTH_ASSERT(write_text(w, "abcd").written() == 4u);
  NTH_EXPECT(s.size() == 4u);
  NTH_EXPECT(s == "abcd");
  NTH_ASSERT(write_text(w, "efgh").written() == 4u);
  NTH_EXPECT(s == "abcdefgh");
}

}  // namespace
}  // namespace nth::io
