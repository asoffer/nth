# `//nth/io/writer`

## Overview

This target defines concepts related to writing data in a serialized format. In particular, it
defines the `nth::io::writer` and `nth::io::write_result_type` concepts. Writers are fundamental to
much of `nth`'s design, including [formatting](/format/format) and [logging](/log/log).

As writers abstract the writing of both data (to be serialized) and text (to be logged). One might
expect a writer to accept a `std::string_view` but this is fundamentally the wrong type for such a
generic interface. Instead, a writer traffics in `std::span<std::byte const>`. One may produce a
`std::span<std::byte>` viewing the bytes of an object via [`nth::bytes`](/memory/bytes), or view a
sequence of bytes represented by an object (such as `std::string` or `std::string_view` via
[`nth::byte_range`](/memory/bytes).

Because writing data may not succeed, we must also represent the notion of a "write result." This is
formalized via the `nth::io::write_result_type` concept.

## The `writer` concept

Formally, `writer` is a concept requiring the type `W` to contain a `write` member function that can
be invoked with a `std::span<std::byte const>`. The function is responsible for writing the span of
bytes in accordance with how the type `W` defines "write." The function must return a
`nth::io::write_result<W>` indicating how many bytes were written. For an argument `byte_span`
producing a returned result of `result`,

* `result.written()` must be greater than or equal to zero,
* `result.written()` must be less than or equal to `byte_span.size()`.
* Callers may interpret `result.written() == byte_span.size()` as a successful write,
  `result.written() == 0` as a failure, and all other possibilities as partial success.

## The `write_result_type` concept

The type returned by a call to `write` on a writer must be something adhering to
`nth::io::write_result_type`. Types adhere to this concept provided they have a `written` const
member function taking no arguments and returning some integral type. The returned value represents
the number of bytes written by a call to `write`.

## Write result customization.

By default, a `writer`'s associated write result type is `nth::io::basic_write_result`. This is a
minimal, type adhering to the `write_result_type` concept, outlined here:

```
struct basic_write_result {
  explicit constexpr basic_write_result(size_t n);
  [[nodiscard]] constexpr size_t written() const;
 private:
  // ...
};
```

Type authors may customize their associated write result type by providing a nested type name
`nth_write_result_type`. This type must be adhere to the `write_result_type` concept and will be
used as the associated write result type.

One can query the write result for a writer type `W` via `nth::io::write_result<W>`.

## Examples

There are two builtin `nth::io::writers` of note:

* [nth::io::string_writer](/io/writer/string), for which calls to `write` append data to a
  `std::string`, and
* [nth::io::file_writer](/io/writer/file), for which calls to `write` write the data to a
  file.
