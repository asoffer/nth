#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <string_view>

#include "nth/base/section.h"
#include "nth/debug/source_location.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"

namespace nth {

// A `log_line` represents a location in source where logging may occur (often
// due to `NTH_LOG`. Log lines must be constructed with the annotation
// `NTH_PLACE_IN_SECTION(nth_log_line)`
struct log_line {
  consteval log_line(nth::source_location loc = nth::source_location::current())
      : source_location_(loc) {}
  consteval log_line(std::string_view verbosity_path,
                     nth::source_location loc = nth::source_location::current())
      : verbosity_path_(verbosity_path), source_location_(loc) {}

  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<log_line>) {
    if constexpr (S.empty()) {
      return nth::trivial_formatter{};
    } else {
      return nth::interpolating_formatter<S>{};
    }
  }

  template <nth::interpolation_string S>
  friend void NthFormat(io::writer auto &w, nth::interpolating_formatter<S> &,
                        log_line const &line) {
    nth::interpolate<S>(w, line.source_location().file_name(),
                        line.source_location().line(),
                        line.source_location().function_name());
  }

  [[nodiscard]] constexpr size_t id() const;

  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  [[nodiscard]] constexpr std::string_view verbosity_path() const {
    return verbosity_path_;
  }

  constexpr bool enabled() const {
    return enabled_.load(std::memory_order::relaxed);
  }

  constexpr void enable(bool b = true) {
    enabled_.store(b, std::memory_order::relaxed);
  }

  constexpr void disable() {
    enabled_.store(false, std::memory_order::relaxed);
  }

 private:
  std::string_view verbosity_path_;
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
