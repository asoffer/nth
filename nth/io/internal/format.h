#ifndef NTH_IO_INTERNAL_FORMAT_H
#define NTH_IO_INTERNAL_FORMAT_H

#include <cstdlib>
#include <string_view>

namespace nth::internal_format {

inline constexpr void ValidateFormatString(std::string_view format_string) {
  int brace_depth = 0;
  for (char c : format_string) {
    switch (c) {
      case '{': {
        if (brace_depth == 1) {
          std::abort();  // Nested braces.
        }
        ++brace_depth;
      } break;
      case '}': {
        if (brace_depth == 0) {
          std::abort();  // `}` without matching `{`.
        }
        --brace_depth;
      } break;
      default: {
        if (brace_depth == 1) {
          std::abort();  // `{` must be followed by `}`.
        }
      } break;
    }
  }
  if (brace_depth != 0) {
    std::abort();  // Mismatched braces.
  }
}

// Represents the range of code units to be replaced.
struct PlaceholderRange {
  int start  = -1;
  int length = -1;
};

template <auto Fmt>
constexpr auto Replacements() {
  std::array<PlaceholderRange, Fmt.placeholders()> array;

  auto* array_ptr = array.data();
  char const* p   = Fmt.NthInternalFormatStringDataMember;
  for (size_t i = 0; i < Fmt.size(); ++i) {
    switch (p[i]) {
      case '{': array_ptr->start = i; break;
      case '}': {
        array_ptr->length = i - array_ptr->start + 1;
        ++array_ptr;
      } break;
    }
  }
  return array;
}

}  // namespace nth::internal_format

#endif  // NTH_IO_INTERNAL_FORMAT_H
