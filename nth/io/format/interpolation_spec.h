#ifndef NTH_IO_FORMAT_INTERPOLATION_SPEC_H
#define NTH_IO_FORMAT_INTERPOLATION_SPEC_H

#include <cstddef>
#include <string_view>

#include "nth/strings/interpolate/string.h"

namespace nth::io {

struct interpolation_spec {
  interpolation_spec() = default;

  explicit interpolation_spec(interpolation_string_view s)
      : interpolation_string_(s) {}

  template <interpolation_string S>
  static constexpr interpolation_spec from() {
    return interpolation_spec(S);
  }

  std::string_view next_chunk(interpolation_spec &child) {
    auto [leading_text, first_field] = interpolation_string_.extract_first();
    child.interpolation_string_      = first_field;
    return leading_text;
  }

  bool empty() const { return interpolation_string_.empty(); }

  constexpr operator nth::interpolation_string_view() const {
    return interpolation_string_;
  }

  std::string_view last_chunk() const {
    return std::string_view(interpolation_string_);
  }

 private:
  ::nth::interpolation_string_view interpolation_string_;
};

}  // namespace nth::io

#endif  // NTH_IO_FORMAT_INTERPOLATION_SPEC_H
