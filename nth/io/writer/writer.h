#ifndef NTH_IO_WRITER_WRITER_H
#define NTH_IO_WRITER_WRITER_H

#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>

#include "nth/meta/concepts/core.h"

namespace nth::io {

// A `write_result_type` is a concept defining the return type of calls to
// `write` on a `writer` defined below. Writing data can fail and a
// `write_result_type` must be capable of differentiating between:
//
//   * The data was written successfully.
//
//   * The writer failed to write the data, and the state of the writer is as it
//     was before the attempt to write was made.
//
//   * The write was partially successful; only the first `n` bytes that
//     were attempted to be written were actually written.
//
// The const member function `written` returns the number of bytes written by
// the call to `write`, which will always be between zero and the size of the
// span of bytes passed to the writer (inclusive).
template <typename R>
concept write_result_type = requires(R const r) {
  { r.written() } -> std::integral;
};

// `basic_write_result` is a simple type conforming to the `write_result_type`
// concept. Authors of `writer` types (see concept definition below) may
// customize their associated `write_result_type`, but `basic_write_result` is
// the default if no customization is provided. A `basic_write_result` can be
// constructed with the number of bytes written, a value which may be queried
// via the `written` member function.
struct basic_write_result {
  explicit constexpr basic_write_result(size_t n) : written_(n) {}
  [[nodiscard]] constexpr size_t written() const { return written_; }

 private:
  size_t written_;
};

namespace internal_writer {
template <typename>
struct write_result {
  using type = basic_write_result;
};
template <typename W>
  requires requires { typename W::nth_write_result_type; }
struct write_result<W> {
  using type = typename W::nth_write_result_type;
};

}  // namespace internal_writer

// An alias for the `write_result_type` associated with the writer `W`. This is
// `typename W::nth_write_result_type` if such a nested typename exists, and
// `nth::io::basic_write_result` otherwise.
template <typename W>
using write_result = internal_writer::write_result<W>::type;

// A `writer` is a concept defining an object into which one can write data
// serially.
template <typename W>
concept writer = requires(W w) {
  requires write_result_type<write_result<W>>;

  // There must be a `write` member function that can be invoked with a
  // `std::span<std::byte const>`. The function is responsible for writing the
  // span of bytes in accordance with how the type `W` defines "write." The
  // function must return a `write_result<W>` indicating how many bytes were
  // written. For an argument `byte_span` producing a returned result of
  // `result`,
  //
  //   * `result.written()` must be greater than or equal to zero,
  //   * `result.written()` must be less than or equal to `byte_span.size()`.
  //   * Callers may interpret `result.written() == byte_span.size()` as a
  //     successful write, `result.written() == 0` as a failure, and all other
  //     possibilities as partial success.
  {
    w.write(nth::value<std::span<std::byte const>>())
  } -> nth::precisely<write_result<W>>;
};

template <writer W>
write_result<W> write(W& w, std::span<std::byte const> bytes) {
  return w.write(bytes);
}

// Because writing text is such a common need, `write_text` is a wrapper around
// `write` providing this ergonomic benefit.
template <writer W>
write_result<W> write_text(W& w, std::string_view text) {
  return w.write(std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(text.data()), text.size()));
}

// `minimal_writer` is the most trivial type satisfying the `writer` concept.
// Its member functions are not implemented as it is intended only for use at
// compile-time with type-traits. `minimal_writer` is intended to be used as a
// stand-in for compile-time checks where a generic writer is needed: If a
// member function can be instantiated with `minimal_writer`, it should be
// instantiatiable with any `writer`.
struct minimal_writer {
  write_result<minimal_writer> write(
      nth::precisely<std::span<std::byte const>> auto);
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_WRITER_H
