#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <cstddef>
#include <string>
#include <vector>

#include "nth/configuration/log.h"
#include "nth/debug/log/internal/component_iterator.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/interpolate.h"

namespace nth {

// A particular invocation of a `log_line` along with encoded arguments.
struct log_entry {
  internal_debug::log_entry_component_iterator component_begin() const;
  internal_debug::log_entry_component_iterator component_end() const;

  log_line_id id() const { return id_; }

  template <size_t>
  friend struct nth::internal_debug::log_line_with_arity;

  log_entry() = delete;
  explicit log_entry(log_line_id id, size_t placeholders);

  std::string& data() & { return data_; }

  void demarcate() { offsets_.push_back(data_.size()); }

 private:
  log_line_id id_;
  std::string data_;
  std::vector<size_t> offsets_ = {0};
};

namespace internal_debug {

template <size_t PlaceholderCount>
Voidifier log_line_with_arity<PlaceholderCount>::operator<<=(
    NTH_ATTRIBUTE(lifetimebound)
        InterpolationArguments<PlaceholderCount> const& e) const {
  log_entry entry(id(), PlaceholderCount);

  bounded_string_printer printer(entry.data(), nth::config::log_print_bound);

  std::apply(
      [&](auto... entries) {
        ((nth::io::print(
              interpolating_printer<"{}", bounded_string_printer>(printer),
              entries),
          entry.demarcate()),
         ...);
      },
      e.entries());

  for (auto* sink : registered_log_sinks()) { sink->send(*this, entry); }

  return Voidifier();
}

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
