#ifndef NTH_IO_SERIALIZE_SERIALIZE_H
#define NTH_IO_SERIALIZE_SERIALIZE_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <type_traits>

#include "nth/io/internal/sequence.h"
#include "nth/io/serialize/internal/serialize.h"
#include "nth/io/writer/writer.h"
#include "nth/utility/bytes.h"

namespace nth::io {

// An alias template that extracts the result type associated with the
// serializer `S`. By default, the result type is `bool`, where `true` indicates
// successful serialization. Users may override this choice by adding a member
// type named `nth_serializer_result_type` to their serializer. The type must be
// convertible to `bool` in such a way that serialization is understood to be
// successful if and only if the result value converts to `true`. Move
template <typename S>
using serializer_result_type = internal_serialize::serializer_result<S>::type;

// A concept indicating that a type `T` can be serialized with a serializer `S`.
template <typename T, typename S>
concept serializable_with = requires(S& s, T const& value) {
  { NthSerialize(s, value) } -> std::convertible_to<serializer_result_type<S>>;
};

// Serializes a sequence of `values...` with the serializer `S`, one
// immediately after the other.
template <typename S>
serializer_result_type<S> serialize(
    S& s, serializable_with<S> auto const&... values) {
  serializer_result_type<S> result;
  (void)(static_cast<bool>(result = NthSerialize(s, values)) and ...);
  return result;
}

// Writes a length-prefixed sequence of the elements in `seq`. The call to
// `std::size(seq)` must return the number of elements iterated over when used
// as the range-expression in a range-for loop.
template <typename S, typename Seq>
serializer_result_type<S> NthSerialize(S& s, as_sequence<Seq> sequence) {
  auto const& seq = sequence.ref();
  if (not nth::io::write_integer(s, std::size(seq))) { return false; }
  for (auto const& elem : seq) {
    if (not nth::io::serialize(s, elem)) { return false; }
  }
  return true;
}

}  // namespace nth::io

#endif  // NTH_IO_SERIALIZE_SERIALIZE_H
