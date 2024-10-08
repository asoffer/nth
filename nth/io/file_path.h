#ifndef NTH_IO_FILE_PATH_H
#define NTH_IO_FILE_PATH_H

#include <optional>
#include <string>
#include <string_view>

#include "nth/base/core.h"
#include "nth/format/format.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"

namespace nth::io {

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
    if (p.has_value()) { path = NTH_MOVE(*p); }
    return p.has_value();
  }

  // Returns a reference to a `std::string const` which represents the file
  // path.
  std::string const &path() const { return name_; }

  // Use the same interpolation strings as `std::string_view`.
  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<file_path>) {
    return NthInterpolateFormatter<S>(nth::type<std::string_view>);
  }

  friend void NthFormat(io::writer auto &w, auto &fmt, file_path const &path) {
    nth::format(w, fmt, path.name_);
  }

  friend bool operator==(file_path const &, file_path const &) = default;
  friend bool operator!=(file_path const &, file_path const &) = default;

 private:
  friend struct file;

  std::string name_;
};

}  // namespace nth::io

#endif  // NTH_IO_FILE_PATH_H
