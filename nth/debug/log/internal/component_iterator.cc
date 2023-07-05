#include "nth/debug/log/internal/component_iterator.h"

namespace nth::internal_debug {

log_entry_component_iterator& log_entry_component_iterator::operator++() {
  ++offset_;
  return *this;
}

log_entry_component_iterator log_entry_component_iterator::operator++(int) {
  auto copy = *this;
  ++*this;
  return copy;
}

std::string_view log_entry_component_iterator::operator*() const {
  size_t start = *(offset_ - 1);
  size_t end   = *offset_;
  return std::string_view(s_ + start, end - start);
}

}  // namespace nth::internal_debug
