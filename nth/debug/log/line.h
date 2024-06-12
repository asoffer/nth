#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <string_view>

#include "nth/debug/source_location.h"
#include "nth/io/format/format.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/string.h"

namespace nth {

struct log_line_id;

struct log_line_base {
  explicit log_line_base();

  // A unique identifier associated with this log line.
  log_line_id id() const;

  // Metadata pertaining to the debug-log line including where it was
  // encountered, invocation time, thread-id, etc.
  struct metadata {
    explicit metadata(struct source_location loc) : source_location_(loc) {}

    // The location in source at which the log occurs.
    struct source_location source_location() const {
      return source_location_;
    }

    template <io::printer_type P>
    friend void NthFormat(P printer, io::format_spec<metadata>,
                          metadata const& metadata) {
      P::print(printer, {}, metadata.source_location().file_name());
      P::print(printer, {}, metadata.source_location().function_name());
      P::print(printer, {}, metadata.source_location().line());
    }

   private:
    struct source_location source_location_;
  };

  // Returns metadata pertaining about where and when the debug-log line
  // occurred.
  struct metadata metadata() const {
    return metadata_;
  }

 protected:
  friend struct log_line_id;

  explicit log_line_base(source_location location)
      : metadata_(std::move(location)) {}

  size_t id_;
  struct metadata metadata_;

  static std::atomic<log_line_base const*> head_;
  static log_line_base const stub_;
};

// `log_line_id` uniquely identifies an instantiation of a logging line. Two
// invocations of the same `NTH_LOG` will have the same `log_line_id`. Notably,
// within a template, instantiations with different template arguments will
// compare unequal.
struct log_line_id {
  constexpr size_t value() const;

  friend constexpr bool operator==(log_line_id, log_line_id) = default;

  constexpr log_line_base const& line() const { return *line_; }

 private:
  friend struct log_line_base;

  explicit constexpr log_line_id() = default;
  explicit constexpr log_line_id(log_line_base const* line) : line_(line) {}
  log_line_base const* line_ = nullptr;
};

// Represents a particular invocation of a log message.
template <interpolation_string S>
struct log_line : log_line_base {
  // The interpolation string to which log arguments are passed.
  constexpr std::string_view interpolation_string() const { return S; }

  friend log_line_id;

  explicit log_line(source_location location = source_location::current())
      : log_line_base(std::move(location)) {}
};

constexpr size_t log_line_id::value() const { return line_->id_; }

inline log_line_id log_line_base::id() const { return log_line_id(this); }

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
