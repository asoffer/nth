#include "nth/io/string_reader.h"

#include <string_view>

#include "nth/io/reader_test.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_INVOKE_TEST("nth/io/reader/**") {
  co_yield TestArguments{type<string_reader>, ""};
  co_yield TestArguments{type<string_reader>, std::string_view("")};
  co_yield TestArguments{type<string_reader>, std::string("")};

  co_yield TestArguments{type<string_reader>, "abcdef"};
  co_yield TestArguments{type<string_reader>, std::string_view("abcdef")};
  co_yield TestArguments{type<string_reader>, std::string("abcdef")};

  co_yield TestArguments{type<string_reader>, "abcdefghijklmnopqrstuvwxyz"};
  co_yield TestArguments{type<string_reader>,
                         std::string_view("abcdefghijklmnopqrstuvwxyz")};
  co_yield TestArguments{type<string_reader>,
                         std::string("abcdefghijklmnopqrstuvwxyz")};
}

NTH_TEST("string_reader/read-bytes") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  string_reader r(input);
  std::array<std::byte, 4> bs;
  auto c = r.cursor();
  NTH_ASSERT(r.read(bs));
  NTH_EXPECT(bs == std::array{
                       std::byte{'a'},
                       std::byte{'b'},
                       std::byte{'c'},
                       std::byte{'d'},
                   });
  NTH_ASSERT(r.cursor() - c == 4);
  std::array<std::byte, 40> too_many_bytes;
  NTH_ASSERT(not r.read(too_many_bytes));
  NTH_ASSERT(r.cursor() - c == 4);
}

NTH_TEST("string_reader/read-bytes-at") {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz";
  string_reader r(input);
  std::array<std::byte, 4> bs;
  auto c = r.cursor();
  r.skip(4);
  NTH_ASSERT(r.read_at(c, bs));
  NTH_EXPECT(bs == std::array{
                       std::byte{'a'},
                       std::byte{'b'},
                       std::byte{'c'},
                       std::byte{'d'},
                   });
  NTH_ASSERT(r.cursor() - c == 4);
  std::array<std::byte, 100> too_many_bytes;
  NTH_EXPECT(not r.read_at(c, too_many_bytes));
  NTH_ASSERT(r.cursor() - c == 4);
}

}  // namespace
}  // namespace nth::io
