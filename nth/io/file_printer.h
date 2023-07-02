#ifndef NTH_IO_FILE_PRINTER_H
#define NTH_IO_FILE_PRINTER_H

#include <cstdio>
#include <string_view>

namespace nth {

struct file_printer {
  explicit constexpr file_printer(std::FILE* f) : file_(f) {}
  void write(size_t n, char c);
  void write(std::string_view s) { std::fwrite(s.data(), 1, s.size(), file_); }

 private:
  std::FILE* file_;
};

inline auto const & stderr_printer = file_printer(stderr);
inline auto const & stdout_printer = file_printer(stdout);

}  // namespace nth

#endif  // NTH_IO_FILE_PRINTER_H
