#ifndef NTH_STRINGS_INTERNAL_INTERPOLATE_H
#define NTH_STRINGS_INTERNAL_INTERPOLATE_H

#include <bit>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <vector>

namespace nth::internal_interpolate {

// Represents the range of code units to be replaced.
struct placeholder_range {
  int start  = -1;
  int length = -1;
};

template <auto InterpolationString>
constexpr void Replacements(placeholder_range* array_ptr) {
  char const* p = std::string_view(InterpolationString).data();
  for (size_t i = 0; i < InterpolationString.size(); ++i) {
    switch (p[i]) {
      case '{': array_ptr->start = i; break;
      case '}': {
        array_ptr->length = i - array_ptr->start + 1;
        ++array_ptr;
      } break;
    }
  }
}

inline std::vector<placeholder_range> Replacements(
    std::string_view interpolation_string) {
  std::vector<placeholder_range> result;
  placeholder_range* ptr;
  char const* p = interpolation_string.data();
  for (size_t i = 0; i < interpolation_string.size(); ++i) {
    switch (p[i]) {
      case '{':
        ptr        = &result.emplace_back();
        ptr->start = i;
        break;
      case '}': {
        ptr->length = i - ptr->start + 1;
      } break;
    }
  }
  return result;
}

}  // namespace nth::internal_interpolate

#endif  // NTH_STRINGS_INTERNAL_INTERPOLATE_H
