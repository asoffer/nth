#include "nth/io/file_printer.h"

#include <cstdio>

#include "nth/debug/trace.h"

bool WritesToFile() {
  std::FILE* f = std::tmpfile();
  NTH_EXPECT(f != nullptr) else {
    std::perror("Failed to open file.");
    return false;
  }

  nth::file_printer fp(f);
  fp.write("Hello, world");
  fp.write(10, '!');
  NTH_EXPECT(std::ftell(f) == 22) else { return false; }

  std::rewind(f);

  char buffer[22];
  std::fread(buffer, 1, 22, f);
  NTH_EXPECT(std::string_view(buffer, 22) == "Hello, world!!!!!!!!!!") else {
    return false;
  }

  NTH_EXPECT(std::fclose(f) == 0) else { return false; }
  return true;
}

bool WritesLargeAmountOfData() {
  std::FILE* f = std::tmpfile();
  NTH_EXPECT(f != nullptr) else {
    std::perror("Failed to open file.");
    return false;
  }

  nth::file_printer fp(f);
  fp.write(5000, '!');
  NTH_EXPECT(std::ftell(f) == 5000) else { return false; }

  std::rewind(f);

  char buffer[5000];
  std::fread(buffer, 1, 5000, f);
  NTH_EXPECT(std::string_view(buffer, 5000) == std::string(5000, '!')) else {
    return false;
  }

  NTH_EXPECT(std::fclose(f) == 0) else { return false; }
  return true;
}

int main() {
  NTH_EXPECT(WritesToFile()) else { return 1; }
  NTH_EXPECT(WritesLargeAmountOfData()) else { return 1; }
  return 0;
}
