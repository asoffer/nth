#include "nth/io/deserialize.h"
#include "nth/io/serialize.h"
#include "nth/io/string_reader.h"
#include "nth/io/string_writer.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("round-trip/integer", auto n) {
  decltype(n) m;
  std::string s;

  string_writer w(s);
  NTH_ASSERT(serialize_integer(w, n));

  string_reader r(s);
  NTH_ASSERT(deserialize_integer(r, m));

  NTH_ASSERT(r.size() == 0u);
  NTH_EXPECT((int)m == (int)n);
}

NTH_INVOKE_TEST("round-trip/integer") {
  for (int8_t n : {
           int8_t{0},
           int8_t{1},
           int8_t{-1},
           int8_t{10},
           int8_t{-10},
           std::numeric_limits<int8_t>::max(),
           std::numeric_limits<int8_t>::lowest(),
       }) {
    co_yield n;
  }

  for (uint8_t n : {
           uint8_t{0},
           uint8_t{1},
           uint8_t{10},
           std::numeric_limits<uint8_t>::max(),
       }) {
    co_yield n;
  }

  for (int32_t n : {
           int32_t{0},
           int32_t{1},
           int32_t{-1},
           int32_t{10},
           int32_t{-10},
           int32_t{256},
           int32_t{-256},
           int32_t{257},
           int32_t{-257},
           int32_t{1'000'000},
           int32_t{-1'000'000},
           std::numeric_limits<int32_t>::max(),
           std::numeric_limits<int32_t>::lowest(),
       }) {
    co_yield n;
  }

  for (uint32_t n : {
           uint32_t{0},
           uint32_t{1},
           uint32_t{10},
           uint32_t{256},
           uint32_t{257},
           uint32_t{1'000'000},
           std::numeric_limits<uint32_t>::max(),
       }) {
    co_yield n;
  }
}

}  // namespace
}  // namespace nth::io
