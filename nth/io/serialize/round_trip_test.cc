#include <cstdint>
#include <string>
#include <vector>

#include "nth/container/flyweight_set.h"
#include "nth/io/deserialize/deserialize.h"
#include "nth/io/reader/string.h"
#include "nth/io/serialize/serialize.h"
#include "nth/io/writer/string.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("round-trip/integer", auto n) {
  decltype(n) m;
  std::string s;

  string_writer w(s);
  NTH_ASSERT(format_integer(w, n));

  string_reader r(s);
  NTH_ASSERT(read_integer(r, m));

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

struct Thing {
  int n;
  friend bool NthSerialize(auto &s, Thing const &t) {
    return format_integer(s, t.n);
  }
  friend bool NthDeserialize(auto &d, Thing &t) {
    return read_integer(d, t.n);
  }
  friend bool operator==(Thing, Thing) = default;

  template <typename H>
  friend H AbslHashValue(H h, Thing t) {
    return H::combine(std::move(h), t.n);
  }
};

NTH_TEST("round-trip/sequence/serialize_sequence",
         std::vector<Thing> const &v) {
  std::vector<Thing> round_tripped;
  std::string s;

  string_writer w(s);
  NTH_ASSERT(serialize(w, as_sequence(v)));

  string_reader r(s);
  NTH_ASSERT(deserialize(r, as_sequence(round_tripped)));

  NTH_ASSERT(r.size() == 0u);
  NTH_EXPECT(v == round_tripped);
}

NTH_TEST("round-trip/sequence/as_sequence", std::vector<Thing> const &v) {
  std::vector<Thing> round_tripped;
  std::string s;

  string_writer w(s);
  NTH_ASSERT(serialize(w, as_sequence(v)));

  string_reader r(s);
  NTH_ASSERT(deserialize(r, as_sequence(round_tripped)));

  NTH_ASSERT(r.size() == 0u);
  NTH_EXPECT(v == round_tripped);
}

NTH_INVOKE_TEST("round-trip/sequence/*") {
  co_yield std::vector<Thing>{};
  co_yield std::vector<Thing>{Thing{0}};
  co_yield std::vector<Thing>{Thing{0}, Thing{1}};
  co_yield std::vector<Thing>{Thing{0}, Thing{1}, Thing{2},
                              Thing{3}, Thing{4}, Thing{5}};
}

NTH_TEST("round-trip/unordered", flyweight_set<Thing> const &set) {
  flyweight_set<Thing> round_tripped;
  std::string s;

  string_writer w(s);
  NTH_ASSERT(serialize(w, as_sequence(set)));

  string_reader r(s);
  NTH_ASSERT(deserialize(r, as_sequence(round_tripped)));

  NTH_ASSERT(r.size() == 0u);
  NTH_EXPECT(set.size() == round_tripped.size());
  for (auto const &element : set) {
    NTH_ASSERT(round_tripped.find(element) != round_tripped.end());
  }
}

NTH_INVOKE_TEST("round-trip/unordered") {
  co_yield flyweight_set<Thing>{};
  co_yield flyweight_set<Thing>{Thing{0}};
  co_yield flyweight_set<Thing>{Thing{0}, Thing{1}};
  co_yield flyweight_set<Thing>{Thing{0}, Thing{1}, Thing{2},
                                Thing{3}, Thing{4}, Thing{5}};
}

}  // namespace
}  // namespace nth::io
