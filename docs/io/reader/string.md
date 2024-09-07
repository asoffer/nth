# `//nth/io/reader:string`

## Overview 

This target defines `nth::io::string_reader`, a type conforming to the
[nth::io::sized_reader](/io/reader/reader) concept, which enables users to read data to a
string. The type can be constructed with a mutable reference to a `std::string`. The referenced
string must outlive the `string_reader`. Calls to `read` will append to the referenced string.

## Example usage

```
std::string s = "Hello, world!";
nth::io::string_reader r(s);

char buffer[7] = {0};
auto result1 = nth::io::read_text(r, buffer);
NTH_EXPECT(result1.bytes_read() == 7);
NTH_EXPECT(std::string_view(buffer) == "Hello, ");

auto result2 = nth::io::read_text(r, buffer);
NTH_EXPECT(result2.bytes_read() == 6);

// Note the trailing space, as the last byte was not modified, and retaines its
// previous value from when the buffer held "Hello, ".
NTH_EXPECT(std::string_view(buffer) == "world! ");
```
