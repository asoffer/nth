#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <string_view>

#include "nth/debug/log/internal/interpolation_arguments.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/source_location.h"

namespace nth {

// Forward-declaration of type identifier by `LogLineId` defined below.
struct LogLine;

// `LogLineId` uniquely identifies an instantiation of a logging line. Two
// invocations of the same `NTH_LOG` will have the same `LogLineId`. Notably,
// within a template, instantiations with different template arguments will
// compare unequal.
struct LogLineId {
  // Returns a reference identified `LogLine`.
  constexpr LogLine const& line() const { return *line_; }

  template <int&..., typename T = int>
  constexpr size_t value() const {
    // Implementation note: The template is not used in any meaningful sense. It
    // exists to make `line_` dependent and therefore not eagerly instantiable,
    // so that we can effectively spell `line_->value()` while `LogLine` is
    // still an incomplete type.
    return (T{}, line_)->id_;
  }

  friend constexpr bool operator==(LogLineId, LogLineId) = default;
  friend constexpr bool operator!=(LogLineId, LogLineId) = default;

 private:
  friend struct LogLine;

  explicit constexpr LogLineId(LogLine const* line = nullptr) : line_(line) {}
  LogLine const* line_ = nullptr;
};

// Represents a particular invocation of a log message.
struct LogLine {

  // The location in source at which the log occurs.
  struct source_location source_location() const {
    return source_location_;
  }

  // The interpolation string to which log arguments are passed.
  std::string_view interpolation_string() const {
    return interpolation_string_;
  }

  // A unique identifier associated with this log line.
  LogLineId id() const { return LogLineId(this); }

 protected:
  explicit LogLine(std::string_view interpolation_string,
                   struct source_location location);

  std::string_view interpolation_string_;
  struct source_location source_location_;
  size_t id_;
  LogLine const* next_ = nullptr;

  static std::atomic<LogLine const*> head_;
  static LogLine const stub_;
};

namespace internal_debug {

template <size_t PlaceholderCount>
struct LogLineWithArity : LogLine {
  explicit LogLineWithArity(std::string_view interpolation_string,
                            struct source_location location)
      : LogLine(interpolation_string, location) {}

  Voidifier operator<<=(
      NTH_ATTRIBUTE(lifetimebound)
          InterpolationArguments<PlaceholderCount> const& entry) const;
};

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
