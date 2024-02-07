#ifndef NTH_IO_SERIALIZE_READER_H
#define NTH_IO_SERIALIZE_READER_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>

namespace nth::io {

// Concept defining a `read_cursor`, representing the location relative to a
// `reader` at which data can be read. A `read_cursor` must be
// equality-comparable and must be subtractable from itself, yielding a
// `ptrdiff_t`.
template <typename C>
concept read_cursor = std::equality_comparable<C> and requires(C const& c) {
  { c - c } -> std::same_as<ptrdiff_t>;
};

// Concept defining a `reader`, representing an object from which one can
// extract data.
template <typename R>
concept reader = requires(R mutable_reader, R const& const_reader) {
  // There must be a `cursor_type` nested type which meets the requirements of
  // a `read_cursor`.
  typename R::cursor_type;
  requires read_cursor<typename R::cursor_type>;

  {
    // There must be a `read_at` member function accepting a `cursor_type`
    // specifying the location to be read from, and accept a span into which
    // data should be read. If the function returns `false`, we say the read was
    // "unsuccessful", and there are no guarantees on the contents of the span.
    // If the function returns `true`, we say the read was "successful" and the
    // span contains the "contents that were read," a notion that must be
    // defined by the implementation of the `reader`.
    const_reader.read_at(std::declval<typename R::cursor_type>(),
                         std::declval<std::span<std::byte>>())
    } -> std::same_as<bool>;

  {
    // There must be an overload of `read` which reads from current cursor (as
    // computed by `const_reader.cursor()`), filling the contents of the span
    // argument. If the function returns `false`, we say the read was
    // "unsuccessful", and there are no guarantees on the contents of the span.
    // If the function returns `true`, we say the read was "successful" and the
    // span contains the "contents that were read," a notion that must be
    // defined by the implementation of the `reader`. The cursor must be moved
    // forward by the size of the span argument.
    mutable_reader.read(std::declval<std::span<std::byte>>())
    } -> std::same_as<bool>;

  {
    // If the function returns `false`, we say the skip was "unsuccessful", and
    // there are no guarantees are made on the state of the reader. If the
    // function returns `true`, we say the skip was "successful". In this
    // situation, the cursor must be moved forward by the size given by the
    // argument.
    mutable_reader.skip(size_t{})
    } -> std::same_as<bool>;

  {
    // Must return the number of bytes left to be read.
    const_reader.size()
    } -> std::same_as<size_t>;

  {
    // Must return the current location of the cursor.
    const_reader.cursor()
    } -> std::same_as<typename R::cursor_type>;
};

}  // namespace nth::io

#endif  // NTH_IO_SERIALIZE_READER_H
