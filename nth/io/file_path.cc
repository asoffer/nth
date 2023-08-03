#include "nth/io/file_path.h"

#include "nth/debug/debug.h"

namespace nth {

std::optional<file_path> file_path::try_construct(std::string_view name) {
  file_path path;
  while (not name.empty()) {
    size_t index = name.find('/');
    if (index == std::string_view::npos) { index = name.size(); }
    std::string_view s = name.substr(0, index);
    if (s.empty()) {
      return std::nullopt;
    } else if (s == ".") {
    } else if (s == "..") {
      size_t rindex = path.name_.rfind('/');
      if (rindex == std::string_view::npos) {
        if (path.name_.empty()) { return std::nullopt; }
        path.name_.clear();
      } else {
        NTH_ASSERT(rindex != size_t{0});
        path.name_.resize(rindex);
      }

    } else {
      if (not path.name_.empty()) { path.name_.push_back('/'); }
      path.name_.append(s);
    }
    name = name.substr(std::min(index + 1, name.size()));
  }
  return path;
}

}  // namespace nth
