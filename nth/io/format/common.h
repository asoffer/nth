#ifndef NTH_IO_FORMAT_COMMON_H
#define NTH_IO_FORMAT_COMMON_H

#include <cstddef>
#include <cstring>
#include <string_view>

#include "nth/io/format/format.h"

namespace nth::io {

// An object solely used for formatting which formats as the given character
// repeated the given number of times.
struct char_spacer {
  explicit char_spacer(char c, size_t count) : content_(c), count_(count) {}

  template <nth::interpolation_string>
  friend nth::io::trivial_formatter NthInterpolateFormatter(
      nth::type_tag<char_spacer>) {
    return {};
  }

  friend void NthFormat(nth::io::writer auto& w, auto&, char_spacer s) {
    char buffer[256];
    std::memset(buffer, s.content_, s.count_ < 256 ? s.count_ : size_t{256});

    size_t iters = s.count_ >> 8;
    for (size_t i = 0; i < iters; ++i) { nth::io::write_text(w, buffer); }
    nth::io::write_text(w, std::string_view(buffer, s.count_ - (iters << 8)));
  }

 private:
  char content_;
  size_t count_;
};

// An object solely used for formatting which formats as the given
// `std::string_view` repeated the given number of times.
struct string_view_spacer {
  explicit string_view_spacer(std::string_view s, size_t count)
      : content_(s), count_(count) {}
  template <nth::interpolation_string>
  friend nth::io::trivial_formatter NthInterpolateFormatter(
      nth::type_tag<string_view_spacer>) {
    return {};
  }

  friend void NthFormat(nth::io::writer auto& w, auto&, string_view_spacer s) {
    for (size_t i = 0; i < s.count_; ++i) {
      nth::io::write_text(w, s.content_);
    }
  }

  std::string_view content_;
  size_t count_;
};

}  // namespace nth::io

#endif  // NTH_IO_FORMAT_COMMON_H
