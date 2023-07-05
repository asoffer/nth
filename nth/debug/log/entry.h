#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <cstddef>
#include <string>
#include <vector>

#include "nth/debug/log/internal/component_iterator.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"

namespace nth {

// A particular invocation of a `LogLine` along with encoded arguments.
struct LogEntry {
  internal_debug::log_entry_component_iterator component_begin() const;
  internal_debug::log_entry_component_iterator component_end() const;

  LogLineId id() const { return id_; }

 private:
  template <size_t>
  friend struct nth::internal_debug::LogLineWithArity;

  LogEntry() = delete;
  explicit LogEntry(LogLineId id, size_t placeholders);

  std::string& data() & { return data_; }

  void demarcate() { offsets_.push_back(data_.size()); }

  LogLineId id_;
  std::string data_;
  std::vector<size_t> offsets_ = {0};
};

namespace internal_debug {

template <size_t PlaceholderCount>
Voidifier LogLineWithArity<PlaceholderCount>::operator<<=(
    NTH_ATTRIBUTE(lifetimebound)
        InterpolationArguments<PlaceholderCount> const& entry) const {
  LogEntry log_entry(id(), PlaceholderCount);

  constexpr size_t bound = 1024;
  bounded_string_printer printer(log_entry.data(), bound);

  auto formatter = nth::config::default_formatter();
  std::apply(
      [&](auto... entries) {
        ((formatter(printer, entries), log_entry.demarcate()), ...);
      },
      entry.entries());

  for (auto* sink : RegisteredLogSinks()) { sink->send(*this, log_entry); }

  return Voidifier();
}

}  // namespace internal_debug
}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
