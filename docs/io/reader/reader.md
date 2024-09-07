# `//nth/io/reader`

## Overview

This target defines concepts related to writing data in a serialized format. In particular, it
defines the `nth::io::reader` and `nth::io::read_result_type` concepts.

As readers abstract the reading of both data (to be serialized) and text (to be parsed). One might
expect a reader to accept a `std::span<char>` that data can be read into, but this is fundamentally
the wrong type for such a generic interface. Instead, a reader traffics in `std::span<std::byte>`.
To address this common need, this header also provides a `read_text` free function, accepting a
`reader` reference and a `std::span<char>`, which converts the `std::span<char>` to a span of bytes.
Alternatively one may produce a `std::span<std::byte>` Via one of the functions in the
[memory/bytes](/memory/bytes) header.

Because reading data may not succeed, we must also represent the notion of a "read result." This is
formalized via the `nth::io::read_result_type` concept.

## `reader`

Formally, `reader` is a concept requiring the type `R` to contain a `read` member function that can
be invoked with a `std::span<std::byte>`. The function is responsible for writing data into this
span of bytes in accordance with how the type `R` defines "read." The function must return a
`nth::io::read_result<R>` indicating how many bytes were read. For an argument `byte_span`
producing a returned result of `result`,

* `result.bytes_read()` must be greater than or equal to zero,
* `result.bytes_read()` must be less than or equal to `byte_span.size()`.

## `sized_reader`

A `sized_reader` is a reader that also provides a `size` member function, The function
must return the number of bytes left to be read. (Streams for which this number is not computable
should be represented by `reader`s but not by `sized_reader`s).

## `read_result_type`

The type returned by a call to `read` on a reader must be something adhering to
`nth::io::read_result_type`. Types adhere to this concept provided they have a `bytes_read` const
member function taking no arguments and returning some integral type. The returned value represents
the number of bytes consumed by a call to `read`.

## Read result customization.

By default, a `reader`'s associated read result type is `nth::io::basic_read_result`. This is a
minimal, type adhering to the `read_result_type` concept, outlined here:

```
struct basic_read_result {
  explicit constexpr basic_read_result(size_t n);
  [[nodiscard]] constexpr size_t written() const;

 private:
  // ...
};
```

Type authors may customize their associated read result type by providing a nested type name
`nth_read_result_type`. This type must be adhere to the `read_result_type` concept and will be
used as the associated read result type.

One can query the read result for a reader type `R` via `nth::io::read_result<R>`.

## Examples

There are two builtin `nth::io::readers` of note:

* [nth::io::string_reader](/io/reader/string), for which calls to `read` append data to a
  `std::string`, and
* [nth::io::file_reader](/io/reader/file), for which calls to `read` read the data to a
  file.
