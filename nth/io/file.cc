#include "nth/io/file.h"

#include <cstdio>

#include "nth/debug/trace/trace.h"
#include "nth/utility/no_destructor.h"

namespace nth {
namespace internal_file {
namespace {

static NoDestructor<file> std_out(MakeFile(stdout));
static NoDestructor<file> std_err(MakeFile(stderr));
static NoDestructor<file> std_in(MakeFile(stdin));

}  // namespace

file MakeFile(std::FILE* f) { return file(f); }

}  // namespace internal_file

file& file::out() { return *internal_file::std_out; }
file& file::err() { return *internal_file::std_err; }

size_t file::tell() const { return std::ftell(file_ptr_); }

void file::rewind() { std::rewind(file_ptr_); }

size_t file::size() const {
  std::fpos_t pos;
  if (std::fgetpos(file_ptr_, &pos) != 0) {}
  std::fseek(file_ptr_, 0, SEEK_END);
  size_t result = tell();
  if (std::fsetpos(file_ptr_, &pos) != 0) {}
  return result;
}

bool file::close() {
  if (std::fclose(file_ptr_) == 0) {
    file_ptr_ = nullptr;
    return true;
  } else {
    return false;
  }
}

file& file::operator=(file&& f) {
  NTH_REQUIRE((v.harden), file_ptr_ == nullptr);
  file_ptr_ = std::exchange(f.file_ptr_, nullptr);
  return *this;
}

file::~file() { NTH_REQUIRE((v.harden), file_ptr_ == nullptr); }

file TemporaryFile() {
  std::FILE* f = std::tmpfile();
  NTH_REQUIRE((v.harden), f != nullptr);
  return file(f);
}

std::optional<file> file::read_only(file_path const& path) {
  if (std::FILE* ptr = std::fopen(path.name_.c_str(), "r")) {
    return file(ptr);
  } else {
    return std::nullopt;
  }
}

}  // namespace nth
