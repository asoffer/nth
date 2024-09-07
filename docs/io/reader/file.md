# `//nth/io/reader:file`

## Overview 

This target defines `nth::io::file_reader`, a type conforming to the
[nth::io::reader](/io/reader/reader) concept, which enables users to read data to a file. The
type can be constructed with the static member function `try_open`, by passing it a
[`nth::io::file_path`](/io/file_path). `try_open` will return `std::nullopt` if the file does not
exist.

## Example usage

```
// Reads at most 1024 bytes from the file and returns the contents as a
// `std::string`.
std::optional<std::string> ReadMessage(nth::io::file_path const & path) {
  std::optional r = nth::io::file_reader::try_construct(path);
  if (not r) { return std::nullopt; }

  char buffer[1024];
  auto result = nth::io::read_text(r, buffer);
  return std::string(buffer, result.bytes_read());
}
```
