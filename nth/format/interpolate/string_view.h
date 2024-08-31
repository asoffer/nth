#ifndef NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H
#define NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H

#include <cstddef>
#include <string_view>

#include "nth/base/attributes.h"
#include "nth/format/interpolate/internal/parameter_range.h"
#include "nth/format/interpolate/string.h"

namespace nth {

struct interpolation_string_view {
  constexpr interpolation_string_view() = default;

  template <size_t Length>
  constexpr interpolation_string_view(
      interpolation_string<Length> const& s NTH_ATTRIBUTE(lifetimebound))
      : s_(s), range_(s.tree_range()) {}

  constexpr size_t size() const { return s_.size() - position_; }
  constexpr size_t length() const { return s_.length() - position_; }
  constexpr bool empty() const { return s_.size() == position_; }

  constexpr operator std::string_view() const { return s_.substr(position_); }
  constexpr char operator[](size_t n) const { return s_[n + position_]; }

  friend constexpr bool operator==(interpolation_string_view,
                                   interpolation_string_view) = default;

  friend constexpr bool operator==(interpolation_string_view lhs,
                                   std::string_view rhs) {
    return static_cast<std::string_view>(lhs) == rhs;
  }

  friend constexpr bool operator==(std::string_view lhs,
                                   interpolation_string_view rhs) {
    return lhs == static_cast<std::string_view>(rhs);
  }

  constexpr internal_interpolate::parameter_range const* range_ptr() const {
    return range_;
  }

 private:
  std::string_view s_                                 = "";
  size_t position_                                    = 0;
  internal_interpolate::parameter_range const* range_ = nullptr;
};

}  // namespace nth

#endif  // NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H
