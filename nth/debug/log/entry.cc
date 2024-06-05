#include "nth/debug/log/entry.h"

namespace nth {

log_entry::log_entry(log_line_id id, size_t placeholders) : id_(id) {
  offsets_.reserve(1 + placeholders);
}

internal_debug::log_entry_component_iterator log_entry::component_begin()
    const {
  return internal_debug::log_entry_component_iterator(data_.data(),
                                                      offsets_.begin() + 1);
}

internal_debug::log_entry_component_iterator log_entry::component_end() const {
  return internal_debug::log_entry_component_iterator(data_.data(),
                                                      offsets_.end());
}

}  // namespace nth
