#ifndef NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_RANGE_H
#define NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_RANGE_H

#include <cstdint>
#include <limits>

namespace nth::internal_interpolate {

struct parameter_range {
  int32_t start  = std::numeric_limits<int32_t>::max();
  int32_t length = std::numeric_limits<int32_t>::max();
  int32_t width  = std::numeric_limits<int32_t>::max();

  constexpr int32_t end() const { return start + length; }
};

inline constexpr void populate_tree(std::string_view interpolation_string,
                                    parameter_range* range_ptr,
                                    int32_t* workspace_ptr) {
  parameter_range* range_start = range_ptr;
  for (int32_t i = interpolation_string.size() - 1; i >= 0; --i) {
    switch (interpolation_string[i]) {
      case '}': *workspace_ptr++ = i; break;
      case '{': {
        --workspace_ptr;
        *(range_ptr - 1) = {
            .start  = i + 1,
            .length = *workspace_ptr - i - 1,
            .width  = 0,
        };
        auto* p = range_ptr;
        for (; p != range_start and p->end() < (range_ptr - 1)->end(); ++p) {}
        (range_ptr - 1)->width = p - range_ptr + 1;
        --range_ptr;
        break;
      }
    }
  }
}

}  // namespace nth::internal_interpolate

#endif  // NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_RANGE_H
