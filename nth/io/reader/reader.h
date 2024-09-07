#ifndef NTH_IO_READER_READER_H
#define NTH_IO_READER_READER_H

#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>

#include "nth/meta/concepts/core.h"

namespace nth::io {

// A `read_result_type` is a concept defining the return type of calls to
// `read` on a `reader` defined below. Reading data can fail and a
// `read_result_type` must be capable of differentiating between:
//
//   * The data was read successfully.
//
//   * The reader failed to read the data, and the state of the reader is as it
//     was before the attempt to read was made.
//
//   * The read was partially successful; only the first `n` bytes that
//     were attempted to be read were actually read.
//
// The const member function `bytes_read` returns the number of bytes written by
// the call to `read`, which will always be between zero and the size of the
// span of bytes passed to the reader (inclusive).
template <typename R>
concept read_result_type = requires(R const r) {
  { r.bytes_read() } -> std::integral;
};

// `basic_read_result` is a simple type conforming to the `read_result_type`
// concept. Authors of `reader` types (see concept definition below) may
// customize their associated `read_result_type`, but `basic_read_result` is
// the default if no customization is provided. A `basic_read_result` can be
// constructed with the number of bytes read, a value which may be queried
// via the `read` member function.
struct basic_read_result {
  explicit constexpr basic_read_result(size_t n) : read_(n) {}
  [[nodiscard]] constexpr size_t bytes_read() const { return read_; }

 private:
  size_t read_;
};

namespace internal_reader {
template <typename>
struct read_result {
  using type = basic_read_result;
};
template <typename R>
  requires requires { typename R::nth_read_result_type; }
struct read_result<R> {
  using type = typename R::nth_read_result_type;
};

}  // namespace internal_reader

// An alias for the `read_result_type` associated with the reader `R`. This is
// `typename R::nth_read_result_type` if such a nested typename exists, and
// `nth::io::basic_read_result` otherwise.
template <typename R>
using read_result = internal_reader::read_result<R>::type;

// A `reader` is a concept defining an object into which one can read serial
// data.
template <typename R>
concept reader = requires(R r) {
  requires read_result_type<read_result<R>>;

  // There must be a `read` member function that can be invoked with a
  // `std::span<std::byte>`. The function is responsible for writing the span of
  // bytes in accordance with how the type `R` defines "read." The function must
  // return a `read_result<R>` indicating how many bytes were read. For an
  // argument `byte_span` producing a returned result of `result`,
  //
  //   * `result.bytes_read()` must be greater than or equal to zero,
  //   * `result.bytes_read()` must be less than or equal to `byte_span.size()`.
  //   * Callers may interpret `result.bytes_read() == byte_span.size()` as a
  //     successful write, `result.bytes_read() == 0` as a failure, and all
  //     other possibilities as partial success.
  {
    r.read(nth::value<std::span<std::byte>>())
  } -> nth::precisely<read_result<R>>;
};

template <reader R>
read_result<R> read(R& r, std::span<std::byte const> bytes) {
  return r.read(bytes);
}

// Because reading text is such a common need, `read_text` is a wrapper around
// `read` providing this ergonomic benefit.
template <reader R>
read_result<R> read_text(R& r, std::span<char> text) {
  return r.read(std::span<std::byte>(reinterpret_cast<std::byte*>(text.data()),
                                     text.size()));
}

// `minimal_reader` is the most trivial type satisfying the `reader` concept.
// Its member functions are not implemented as it is intended only for use at
// compile-time with type-traits. `minimal_reader` is intended to be used as a
// stand-in for compile-time checks where a generic reader is needed: If a
// member function can be instantiated with `minimal_reader`, it should be
// instantiatiable with any `reader`.
struct minimal_reader {
  read_result<minimal_reader> read(nth::precisely<std::span<std::byte>> auto);
};

}  // namespace nth::io

#endif  // NTH_IO_READER_READER_H
