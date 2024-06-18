#ifndef NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H
#define NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H

#include <cstddef>
#include <string_view>

#include "nth/base/attributes.h"
#include "nth/format/interpolate/internal/parameter_range.h"

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

  auto begin() const { return std::string_view(*this).begin(); }
  auto end() const { return std::string_view(*this).end(); }

  std::string_view leading_text() const {
    return s_.substr(position_, range_->start - 1 - position_);
  }

  constexpr interpolation_string_view first_field() const {
    interpolation_string_view s;
    s.s_ = s_.substr(0, range_->end());
    s.position_ += range_->start;
    s.range_ = range_ + 1;
    return s;
  }
  struct extract_result;
  extract_result extract_first();
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

struct interpolation_string_view::extract_result {
  std::string_view leading_text;
  interpolation_string_view first_field;
};

inline interpolation_string_view::extract_result
interpolation_string_view::extract_first() {
  extract_result result{
      .leading_text = leading_text(),
      .first_field  = first_field(),
  };
  position_ = range_->end() + 1;
  range_ += range_->width;
  return result;
}

}  // namespace nth

#endif  // NTH_IO_FORMAT_INTERPOLATE_STRING_VIEW_H
