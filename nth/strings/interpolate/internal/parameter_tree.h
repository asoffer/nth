#ifndef NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_TREE_H
#define NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_TREE_H

#include <cstddef>
#include <cstdint>

#include "nth/strings/interpolate/string.h"

namespace nth::internal_interpolate {
struct interpolation_parameter_range {
  int32_t start;
  int32_t length;
  int32_t width;

  constexpr size_t end() const { return start + length; }
};

template <size_t N>
struct interpolation_parameter_tree {
  template <size_t L>
  consteval interpolation_parameter_tree(interpolation_string<L> const& l) {
    int32_t workspace[L / 2];
    int32_t* ptr = &workspace[0];

    interpolation_parameter_range* range_ptr = &ranges_[N];
    for (int32_t i = l.size() - 1; i >= 0; --i) {
      switch (l[i]) {
        case '}': *ptr++ = i; break;
        case '{': {
          --ptr;
          *(range_ptr - 1) = {
              .start  = i + 1,
              .length = *ptr - i - 1,
              .width  = 0,
          };
          auto* p = range_ptr;
          for (; p != &ranges_[N] and p->end() < (range_ptr - 1)->end(); ++p) {}
          (range_ptr - 1)->width = p - range_ptr + 1;
          --range_ptr;
          break;
        }
      }
    }
  }

  template <size_t M>
  constexpr interpolation_parameter_range top_level_range() const {
    if constexpr (M == -1) {
      return interpolation_parameter_range{
          .start = -1, .length = 0, .width = 0};
    } else {
      size_t i = 0;
      for (size_t n = 0; n < M; ++n) { i += ranges_[i].width; }
      return ranges_[i];
    }
  }

 private:
  interpolation_parameter_range ranges_[N];
};

template <>
struct interpolation_parameter_tree<0> {
  template <size_t L>
  consteval interpolation_parameter_tree(interpolation_string<L> const&) {}
};

}  // namespace nth::internal_interpolate

#endif  // NTH_STRINGS_INTERPOLATION_INTERNAL_PARAMETER_TREE_H
