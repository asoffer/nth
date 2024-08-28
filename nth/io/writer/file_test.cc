#include "nth/io/writer/file.h"

#include "nth/io/file_path.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("/nth/io/writer/file/open") {
  std::optional f =
      file_path::try_construct("/tmp/nth_io_file_writer_test.txt");
  NTH_ASSERT(f.has_value());

  std::optional w = file_writer::try_open(*f);
  NTH_ASSERT(w.has_value());
}

NTH_TEST("/nth/io/writer/file/open") {
  std::optional f =
      file_path::try_construct("/tmp/nth_io_file_writer_test.txt");
  NTH_ASSERT(f.has_value());

  std::string_view content = "Hello, world!";

  {
    std::optional w = file_writer::try_open(*f);
    NTH_ASSERT(w.has_value());

    NTH_ASSERT(write_text(*w, content).written() == content.size());
  }

  std::FILE* fptr = std::fopen(f->path().c_str(), "r");
  NTH_ASSERT(fptr != nullptr);
  std::string s(1024, '\0');
  NTH_ASSERT(std::fread(s.data(), 1, 1024, fptr) == content.size());
  std::fclose(fptr);
  NTH_EXPECT(std::string(s.c_str()) == content);
}

}  // namespace
}  // namespace nth::io
