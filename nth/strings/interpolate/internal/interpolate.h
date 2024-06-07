#ifndef NTH_STRINGS_INTERPOLATE_INTERNAL_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_INTERNAL_INTERPOLATE_H

#include <cstddef>
#include <string_view>
#include <utility>

#include "nth/meta/type.h"

namespace nth {
namespace internal_interpolate {

constexpr size_t matching_close(std::string_view s, size_t n) {
  char const *p  = &s[n + 1];
  size_t nesting = 1;
  for (; p < s.data() + s.size(); ++p) {
    switch (*p) {
      case '{': ++nesting; break;
      case '}':
        if (--nesting == 0) { return p - s.data(); }
        break;
    }
  }
  return s.size() - 1;
}

}  // namespace internal_interpolate
}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_INTERNAL_INTERPOLATE_H
