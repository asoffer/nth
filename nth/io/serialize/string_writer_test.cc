#include "nth/io/serialize/string_writer.h"

#include <string_view>

#include "nth/io/serialize/writer_test.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

std::span<std::byte const> to_bytes(std::string_view s) {
  return std::span(reinterpret_cast<std::byte const *>(s.data()), s.size());
}

NTH_INVOKE_TEST("nth/io/serialize/writer/**") {
  std::string s;
  co_yield TestArguments{type<string_writer>, s};
}

NTH_TEST("string_writer/basic") {
  std::string s;
  string_writer w(s);
  NTH_EXPECT(s.empty());
}

NTH_TEST("string_writer/allocate") {
  std::string s;
  string_writer w(s);
  auto c = w.allocate(4);
  NTH_ASSERT(c.has_value());
  NTH_EXPECT(w.cursor() - *c == 4);
  NTH_EXPECT(s.size() == 4u);
  w.write_at(*c, to_bytes("abcd"));
  NTH_EXPECT(s.size() == 4u);
  NTH_EXPECT(w.cursor() - *c == 4);
  NTH_EXPECT(s == "abcd");
}

NTH_TEST("string_writer/write") {
  std::string s;
  string_writer w(s);
  NTH_ASSERT(w.write(to_bytes("abcd")));
  NTH_EXPECT(s.size() == 4u);
  NTH_EXPECT(s == "abcd");
  NTH_ASSERT(w.write(to_bytes("efgh")));
  NTH_EXPECT(s == "abcdefgh");
}

}  // namespace
}  // namespace nth::io
