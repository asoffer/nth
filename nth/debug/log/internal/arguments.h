#ifndef NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
#define NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H

#include <vector>

#include "nth/debug/log/entry.h"
#include "nth/debug/log/internal/voidifier.h"
#include "nth/debug/log/line.h"
#include "nth/io/printer.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate/interpolate.h"
#include "nth/strings/interpolate/string.h"

namespace nth::internal_log {

struct entry_builder : io::printer<entry_builder> {
  explicit constexpr entry_builder(
      log_entry& entry NTH_ATTRIBUTE(lifetimebound))
      : entry_(&entry) {}

  void write(std::string_view s) {
    size_t element = entry_->open_element();
    entry_->write(s);
    entry_->close_element(element);
  }
  void write(size_t, char) { std::abort(); }

  [[nodiscard]] size_t start_substructure() { return entry_->open_subtree(); }

  void complete_substructure(size_t n) { entry_->close_subtree(n); }

 private:
  log_entry* entry_;
};

template <compile_time_string File, int Line, interpolation_string S>
inline log_line<S> line(source_location::unchecked(File.data(), "", Line));

// Arguments passed directly to a log macro as in the example:
//
// ```
// NTH_LOG((v.always), "{} + {} == {}") <<= {2, 3, 5};
// ```
//
template <compile_time_string File, int Line, interpolation_string S>
struct arguments {
  template <typename... Ts>
  arguments(Ts const&... ) {
    auto const& log_line = line<File, Line, S>;
    log_entry e(log_line.id());
    entry_builder builder(e);
    // TODO: print_structure<S>(builder, ts...);

    for (auto* sink : internal_debug::registered_log_sinks()) {
      sink->send(log_line, e);
    }
  }
};

struct argument_ignorer {};

template <compile_time_string File, int Line, interpolation_string S>
struct location_injector {
  location_injector() requires(S.placeholders() != 0) = default;

  location_injector() requires(S.placeholders() == 0) {
    auto const& log_line = line<File, Line, S>;
    log_entry e(log_line.id());

    for (auto* sink : internal_debug::registered_log_sinks()) {
      sink->send(log_line, e);
    }
  }

  voidifier operator<<=(argument_ignorer) requires(S.placeholders() == 0) {
    return voidifier{};
  }

  voidifier operator<<=(arguments<File, Line, S>) requires(S.placeholders() !=
                                                           0) {
    return voidifier{};
  }
};

}  // namespace nth::internal_log

#endif  // NTH_DEBUG_LOG_INTERNAL_ARGUMENTS_H
