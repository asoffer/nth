#ifndef NTH_IO_FILE_PATH_H
#define NTH_IO_FILE_PATH_H

#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "nth/io/printer.h"

namespace nth {

// Represents the name of a file. Paths are canonicalized by removing "./" and
// "../".
struct file_path {
  // Constructs a `file_path` with no name.
  file_path() = default;

  // Returns a`file_path` named by `name` if the path can be canonicalized, and
  // `std::nullopt` otherwise.
  static std::optional<file_path> try_construct(std::string_view name);

  friend bool NthCommandlineParse(std::string_view s, file_path &path, auto) {
    std::optional p = file_path::try_construct(s);
    if (p.has_value()) { path = std::move(*p); }
    return p.has_value();
  }

  // Returns a reference to a `std::string const` which represents the file
  // path.
  std::string const &path() const { return name_; }

  template <io::printer_type P>
  friend void NthFormat(P p, file_path const &path) {
    std::stringstream ss;
    ss << std::quoted(path.name_);
    return P::print(std::move(p), ss.str());
  }

  friend bool operator==(file_path const &, file_path const &) = default;
  friend bool operator!=(file_path const &, file_path const &) = default;

 private:
  friend struct file;

  std::string name_;
};

}  // namespace nth

#endif  // NTH_IO_FILE_PATH_H
