#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <string_view>

#include "nth/debug/log/internal/interpolation_arguments.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/source_location.h"

namespace nth {

// Forward-declaration of type identifier by `log_line_id` defined below.
struct log_line;

// `log_line_id` uniquely identifies an instantiation of a logging line. Two
// invocations of the same `NTH_LOG` will have the same `log_line_id`. Notably,
// within a template, instantiations with different template arguments will
// compare unequal.
struct log_line_id {
  // Returns a reference identified `log_line`.
  constexpr log_line const& line() const { return *line_; }

  constexpr size_t value() const;

  friend constexpr bool operator==(log_line_id, log_line_id) = default;

 private:
  friend struct log_line;

  explicit constexpr log_line_id() = default;
  explicit constexpr log_line_id(log_line const* line) : line_(line) {}
  log_line const* line_ = nullptr;
};

// Represents a particular invocation of a log message.
struct log_line {
  // Metadata pertaining to the debug-log line including where it was
  // encountered, invocation time, thread-id, etc.
  struct metadata {
    explicit metadata(struct source_location loc) : source_location_(loc) {}

    // The location in source at which the log occurs.
    struct source_location source_location() const {
      return source_location_;
    }

    friend void NthPrint(auto& printer, auto& formatter,
                         metadata const& metadata) {
      printer.write("\x1b[0;34m");
      formatter(printer, metadata.source_location().file_name());
      printer.write(" ");
      formatter(printer, metadata.source_location().function_name());
      printer.write(":");
      formatter(printer, metadata.source_location().line());
      printer.write("]\x1b[0m ");
    }

   private:
    struct source_location source_location_;
  };

  // Returns metadata pertaining about where and when the debug-log line
  // occurred.
  struct metadata metadata() const {
    return metadata_;
  }

  // The interpolation string to which log arguments are passed.
  std::string_view interpolation_string() const {
    return interpolation_string_;
  }

  // A unique identifier associated with this log line.
  log_line_id id() const { return log_line_id(this); }

 protected:
  friend log_line_id;

  explicit log_line(std::string_view interpolation_string,
                    source_location location);

  std::string_view interpolation_string_;
  struct metadata metadata_;
  size_t id_;

  static std::atomic<log_line const*> head_;
  static log_line const stub_;
};

namespace internal_debug {

struct sink_options_resetter {
  sink_options_resetter() {}
  sink_options_resetter(void* sink, void (*fn)(void*)) : sink_(sink), fn_(fn) {}
  ~sink_options_resetter() { fn_(sink_); }

  template <typename Sink>
  static void erased_set_options(void* s) {
    static_cast<Sink*>(s)->set_options({});
  }

 private:
  void* sink_;
  void (*fn_)(void*);
};

template <size_t PlaceholderCount>
struct log_line_with_arity : log_line {
  explicit log_line_with_arity(std::string_view interpolation_string,
                               source_location location)
      : log_line(interpolation_string, location) {}

  template <typename Sink>
  log_line_with_arity& configure(Sink& sink,
                                 typename Sink::options const& options,
                                 sink_options_resetter&& r = {}) {
    sink.set_options(options);
    r = sink_options_resetter(&sink,
                              sink_options_resetter::erased_set_options<Sink>);
    return *this;
  }

  Voidifier operator<<=(
      NTH_ATTRIBUTE(lifetimebound)
          InterpolationArguments<PlaceholderCount> const& entry) const;
};

}  // namespace internal_debug

constexpr size_t log_line_id::value() const { return line_->id_; }

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
