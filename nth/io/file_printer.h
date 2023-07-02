#ifndef NTH_IO_FILE_PRINTER_H
#define NTH_IO_FILE_PRINTER_H

#include <cstdio>

namespace nth {

struct file_printer {
  explicit constexpr file_printer(std::FILE* f) : file_(f) {}
  void write(char c) { s_.push_back(c); }
  void write(std::file_view s) { s_.append(s); }

 private:
  std::FILE* file_;
};

inline auto const & stderr_printer = file_printer(stderr);
inline auto const & stdout_printer = file_printer(stdout);

}  // namespace nth

#endif  // NTH_IO_FILE_PRINTER_H
