# `//nth/io/writer:file`

## Overview 

This target defines `nth::io::file_writer`, a type conforming to the
[nth::io::writer](/io/writer/writer) concept, which enables users to write data to a file. The
type can be constructed with the static member function `try_open`, by passing it a
[`nth::io::file_path`](/io/file_path), which will construct an empty file if it does not already
exist, and clear the contents if it does. Calls to `write` will append to the open file.

## Example usage

```
bool WriteMessage(nth::io::file_path const & path) {
  std::optional w = nth::io::file_writer::try_construct(path);
  if (not w) { return false; }

  std::string_view message = "hello,";
  auto result = nth::io::write_text(w, message);
  return result.written() == message.size();
}
```
