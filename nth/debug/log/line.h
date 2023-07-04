#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <string_view>

#include "nth/base/attributes.h"
#include "nth/configuration/verbosity.h"
#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/interpolation_arguments.h"
#include "nth/debug/source_location.h"

namespace nth {

// Represents a particular invocation of a log message.
struct LogLine {
  explicit LogLine(std::string_view interpolation_string,
                   struct source_location location);

  struct source_location source_location() const {
    return source_location_;
  }

  std::string_view interpolation_string() const {
    return interpolation_string_;
  }

 protected:
  std::string_view interpolation_string_;
  struct source_location source_location_;
  size_t index_        = 0;
  LogLine const* next_ = nullptr;
  static std::atomic<LogLine const*> head_;
};

struct LogSink;
namespace internal_debug {

template <size_t>
struct MakeDependent {
  friend auto* operator,(MakeDependent, auto* p) { return p; }
};

std::vector<LogSink*>& RegisteredLogSinks();

template <size_t PlaceholderCount>
struct LogLineWithArity : LogLine {
  explicit LogLineWithArity(std::string_view interpolation_string,
                            struct source_location location)
      : LogLine(interpolation_string, location) {}

  friend Voidifier operator<<=(
      LogLineWithArity const& l,
      NTH_ATTRIBUTE(lifetimebound)
          InterpolationArguments<PlaceholderCount> const& entry) {
    LogEntry log_entry;
    log_entry.set_placeholder_count(PlaceholderCount);

    constexpr size_t bound = 1024;
    bounded_string_printer printer(log_entry.data(), bound);

    auto formatter = nth::config::default_formatter();
    std::apply(
        [&](auto... entries) {
          ((formatter(printer, entries), log_entry.demarcate()), ...);
        },
        entry.entries());

    for (auto* logger : RegisteredLogSinks()) {
      (MakeDependent<PlaceholderCount>{}, logger)->send(l, log_entry);
    }

    return Voidifier();
  }
};

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
