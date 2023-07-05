#include "nth/debug/log/entry.h"

namespace nth {

LogEntry::LogEntry(LogLineId id, size_t placeholders) : id_(id) {
  offsets_.reserve(1 + placeholders);
}

internal_debug::log_entry_component_iterator LogEntry::component_begin() const {
  return internal_debug::log_entry_component_iterator(data_.data(),
                                                      offsets_.begin() + 1);
}

internal_debug::log_entry_component_iterator LogEntry::component_end() const {
  return internal_debug::log_entry_component_iterator(data_.data(),
                                                      offsets_.end());
}

}  // namespace nth
