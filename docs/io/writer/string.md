# `//nth/io/writer:string`

## Overview 

This target defines `nth::io::string_writer`, a type conforming to the
[nth::io::writer](/io/writer/writer) concept, which enables users to write data to a string. The
type can be constructed with a mutable reference to a `std::string`. The referenced string must
outlive the `string_writer`. Calls to `write` will append to the referenced string.

## Example usage

```
std::string s;
nth::io::string_writer w(s);

std::string_view message1 = "hello,";
auto result1 = s.write(nth::byte_range(message1));
NTH_EXPECT(result1.written() == message1.size());

std::string_view message2 = " world!";
auto result2 = s.write(nth::byte_range(message1));
NTH_EXPECT(result2.written() == message2.size());

NTH_EXPECT(s == "hello, world!");
```
