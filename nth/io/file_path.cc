#include "nth/io/file_path.h"

#include "nth/debug/debug.h"

namespace nth {

std::optional<file_path> file_path::try_construct(std::string_view name) {
  std::optional<file_path> path;
  path.emplace();
  if (not name.empty() and name[0] == '/') {
    path->name_.push_back('/');
    name.remove_prefix(1);
  }
  while (not name.empty()) {
    size_t index = name.find('/');
    if (index == std::string_view::npos) { index = name.size(); }
    std::string_view s = name.substr(0, index);
    if (s.empty()) {
      path.reset();
      return path;
    } else if (s == ".") {
    } else if (s == "..") {
      size_t rindex = path->name_.rfind('/');
      if (rindex == std::string_view::npos) {
        if (path->name_.empty()) {
          path.reset();
          return path;
        }
        path->name_.clear();
      } else if (rindex == 0) {
        path.reset();
        return path;
      } else {
        path->name_.resize(rindex);
      }

    } else {
      if (not path->name_.empty()) { path->name_.push_back('/'); }
      path->name_.append(s);
    }
    name.remove_prefix(std::min(index + 1, name.size()));
  }
  return path;
}

}  // namespace nth
