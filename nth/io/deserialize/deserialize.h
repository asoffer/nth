#ifndef NTH_IO_DESERIALIZE_DESERIALIZE_H
#define NTH_IO_DESERIALIZE_DESERIALIZE_H

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>

#include "nth/container/free_functions.h"
#include "nth/io/deserialize/internal/deserialize.h"
#include "nth/io/internal/sequence.h"
#include "nth/io/reader/reader.h"
#include "nth/meta/concepts.h"
#include "nth/utility/bytes.h"

namespace nth::io {

// An alias template that extracts the result type associated with the
// deserializer `D`. By default, the result type is `bool`, where `true`
// indicates successful deserialization. Users may override this choice by
// adding a member type named `nth_deserializer_result_type` to their
// deserializer. The type must be convertible to `bool` in such a way that
// deserialization is understood to be successful if and only if the result
// value converts to `true`.
template <typename D>
using deserializer_result_type =
    internal_deserialize::deserializer_result<D>::type;

// A concept indicating that a type `T` can be deserialized with a deserializer
// `D`.
template <typename T, typename D>
concept deserializable_with = requires(D& d, T& value) {
  {
    NthDeserialize(d, value)
    } -> std::convertible_to<deserializer_result_type<D>>;
};

// Deserializes a sequence of `values...` with the deserializer `D`, one
// immediately after the other.
template <typename D>
deserializer_result_type<D>
deserialize(D& d, deserializable_with<D> auto&&... values) requires(
    lvalue_proxy<decltype(values)>and...) {
  deserializer_result_type<D> result;
  (void)(static_cast<bool>(result = NthDeserialize(d, values)) and ...);
  return result;
}

// Deserializes a sequence of `Seq::value_type` into the passed-in `sequence`,
// formatted as a count of the number of elements followed by serializations of
// that many elements.
template <typename D, typename Seq>
deserializer_result_type<D> NthDeserialize(
    D& d, as_sequence<Seq&> sequence) requires(not std::is_const_v<Seq>) {
  auto& seq       = sequence.ref();
  using size_type = decltype(std::size(seq));
  size_type seq_size;
  if (not nth::io::read_integer(d, seq_size)) { return false; }

  if constexpr (requires { nth::reserve(seq, seq_size); }) {
    nth::reserve(seq, seq_size);
  }

  using value_type = std::decay_t<decltype(seq)>::value_type;
  if constexpr (requires {
                  { nth::emplace_back(seq) } -> std::same_as<value_type&>;
                }) {
    for (size_type i = 0; i < seq_size; ++i) {
      if (not nth::io::deserialize(d, nth::emplace_back(seq))) { return false; }
    }
  } else if constexpr (requires {
                         { nth::emplace(seq) } -> std::same_as<value_type&>;
                       }) {
    for (size_type i = 0; i < seq_size; ++i) {
      if (not nth::io::deserialize(d, nth::emplace(seq))) { return false; }
    }
  } else {
    value_type element;
    for (size_type i = 0; i < seq_size; ++i) {
      if (not nth::io::deserialize(d, element)) { return false; }
      nth::insert(seq, element);
    }
  }
  return true;
}

}  // namespace nth::io

#endif  // NTH_IO_DESERIALIZE_DESERIALIZE_H
