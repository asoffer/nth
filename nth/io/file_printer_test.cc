#include "nth/io/file_printer.h"

#include <cstdio>

#include "nth/debug/trace.h"
#include "nth/io/file.h"

bool WritesToFile() {
  nth::file f = nth::TemporaryFile();
  nth::file_printer fp(f.get());

  fp.write("Hello, world");
  fp.write(10, '!');
  NTH_EXPECT(f.tell() == 22u) else { return false; }

  f.rewind();

  char buffer[22];
  std::span<char> span = f.read_into(buffer);
  NTH_EXPECT(std::string_view(span.data(), span.size()) ==
             "Hello, world!!!!!!!!!!")
  else {
    return false;
  }

  NTH_EXPECT(f.close()) else { return false; }
  return true;
}

bool WritesLargeAmountOfData() {
  nth::file f = nth::TemporaryFile();
  nth::file_printer fp(f.get());

  fp.write(5000, '!');
  NTH_EXPECT(f.tell() == 5000u) else { return false; }

  f.rewind();

  char buffer[5000];
  std::span<char> span = f.read_into(buffer);
  NTH_EXPECT(std::string_view(span.data(), span.size()) ==
             std::string(5000, '!'))
  else {
    return false;
  }

  NTH_EXPECT(f.close()) else { return false; }
  return true;
}

int main() {
  NTH_EXPECT(WritesToFile()) else { return 1; }
  NTH_EXPECT(WritesLargeAmountOfData()) else { return 1; }
  return 0;
}
