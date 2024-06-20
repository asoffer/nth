#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>

#include "nth/base/section.h"
#include "nth/debug/source_location.h"
#include "nth/format/interpolate/string_view.h"

namespace nth {

// A `log_line` represents a location in source where logging may occur (often
// due to `NTH_LOG`. Log lines must be constructed with the annotation
// `NTH_PLACE_IN_SECTION(nth_log_line)`
struct log_line {
  consteval log_line(interpolation_string_view str,
                     nth::source_location loc = nth::source_location::current())
      : str_(str), source_location_(loc) {}

  [[nodiscard]] constexpr size_t id() const;

  [[nodiscard]] constexpr interpolation_string_view interpolation_string()
      const {
    return str_;
  }

  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  constexpr bool enabled() const {
    return enabled_.load(std::memory_order::relaxed);
  }

  constexpr void enable(bool b = true) {
    return enabled_.store(b, std::memory_order::relaxed);
  }

  constexpr void disable() {
    return enabled_.store(false, std::memory_order::relaxed);
  }

 private:
  interpolation_string_view str_;
  struct source_location source_location_;
  std::atomic<bool> enabled_ = true;
};

}  // namespace nth

NTH_DECLARE_SECTION(nth_log_line, ::nth::log_line);

namespace nth {

constexpr inline size_t log_line::id() const {
  return this - nth::section<"nth_log_line">.begin();
}

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
