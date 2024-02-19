#ifndef NTH_IO_SERIALIZE_ROUND_TRIP_H
#define NTH_IO_SERIALIZE_ROUND_TRIP_H

#include <concepts>
#include <string>

#include "nth/io/reader/string.h"
#include "nth/io/serialize/deserialize.h"
#include "nth/io/serialize/serialize.h"
#include "nth/io/writer/string.h"
#include "nth/test/test.h"

NTH_TEST("nth/io/serialize/round-trip",
         std::equality_comparable auto const &original) {
  std::decay_t<decltype(original)> object;

  {
    std::string content;

    {
      nth::io::string_writer s(content);
      NTH_ASSERT(nth::io::serialize(s, original));
    }

    {
      nth::io::string_reader d(content);
      NTH_ASSERT(nth::io::deserialize(d, object));
    }
  }

  NTH_EXPECT(original == object);
}

#endif  // NTH_IO_SERIALIZE_ROUND_TRIP_H
