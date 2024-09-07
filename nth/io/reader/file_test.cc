#include "nth/io/reader/file.h"

#include "nth/io/writer/file.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

bool write_file(file_path const& f) {
  auto fw = nth::io::file_writer::try_open(f);
  if (not fw) { return false; }
  return nth::io::write_text(*fw, "Hello, world!").written() == 13;
}

NTH_TEST("/nth/io/reader/file/open/exists") {
  std::optional f =
      file_path::try_construct("/tmp/nth_io_file_reader_test.txt");
  NTH_ASSERT(f.has_value());
  NTH_ASSERT(write_file(*f));

  std::optional r = file_reader::try_open(*f);
  NTH_ASSERT(r.has_value());
}

NTH_TEST("/nth/io/reader/file/open/does-not-exist") {
  std::optional f =
      file_path::try_construct("/tmp/nonexistent_nth_io_file_reader_test.txt");
  NTH_ASSERT(f.has_value());
  std::optional r = file_reader::try_open(*f);
  NTH_ASSERT(not r.has_value());
}

NTH_TEST("/nth/io/reader/file/read") {
  char buffer[1024] = {0};
  std::optional f =
      file_path::try_construct("/tmp/nth_io_file_reader_test.txt");
  NTH_ASSERT(f.has_value());
  NTH_ASSERT(write_file(*f));
  std::optional r = file_reader::try_open(*f);
  NTH_ASSERT(r.has_value());
  NTH_ASSERT(read_text(*r, buffer).bytes_read() == 13u);
  NTH_EXPECT(std::string_view(buffer, 13) == "Hello, world!");
}

}  // namespace
}  // namespace nth::io
