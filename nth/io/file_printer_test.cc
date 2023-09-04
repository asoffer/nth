#include "nth/io/file_printer.h"

#include <cstdio>

#include "nth/io/file.h"
#include "nth/test/test.h"

NTH_TEST("file-printer/writes-to-file") {
  nth::file f = nth::TemporaryFile();
  nth::file_printer fp(f.get());

  fp.write("Hello, world");
  fp.write(10, '!');
  NTH_ASSERT(f.tell() == 22u);

  f.rewind();

  char buffer[22];
  std::span<char> span = f.read_into(buffer);
  NTH_ASSERT(std::string_view(span.data(), span.size()) ==
             "Hello, world!!!!!!!!!!");

  NTH_ASSERT(f.close());
}

NTH_TEST("file-printer/writes-large-amount-of-data-to-file") {
  nth::file f = nth::TemporaryFile();
  nth::file_printer fp(f.get());

  fp.write(5000, '!');
  NTH_ASSERT(f.tell() == 5000u);

  f.rewind();

  char buffer[5000];
  std::span<char> span = f.read_into(buffer);
  NTH_ASSERT(std::string_view(span.data(), span.size()) ==
             std::string(5000, '!'));

  NTH_ASSERT(f.close());
}
