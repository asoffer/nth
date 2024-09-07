#include "nth/io/reader/string.h"

#include <string_view>

#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("string_reader/basic") {
  std::string s = "hello";
  string_reader r(s);
  NTH_EXPECT(s.size() == 5u);
}

NTH_TEST("string_reader/read") {
  std::string s = "abcde";
  string_reader r(s);
  char buffer[4];
  NTH_ASSERT(read_text(r, std::span(buffer)).bytes_read() == 4u);
  NTH_ASSERT(s.size() == 5u);
  NTH_EXPECT(std::string_view(buffer, 4) == "abcd");
  NTH_ASSERT(read_text(r, std::span(buffer)).bytes_read() == 1u);
  NTH_ASSERT(s.size() == 5u);
  NTH_EXPECT(std::string_view(buffer, 4) == "ebcd");
}

}  // namespace
}  // namespace nth::io
