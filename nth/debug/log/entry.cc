#include "nth/debug/log/entry.h"

#include <cstring>

namespace nth {

void log_entry::builder::write(std::string_view s) {
  uint16_t str_size = s.size();
  char buffer[2];
  std::memcpy(buffer, &str_size, sizeof(str_size));
  entry_.data_.append(std::string_view(buffer, 2));
  entry_.data_.append(s);
}

log_entry::const_iterator& log_entry::const_iterator::operator++() {
  uint16_t element_size;
  std::memcpy(&element_size, ptr_, 2);
  ptr_ += element_size + 2;
  return *this;
}

std::string_view log_entry::const_iterator::operator*() const {
  uint16_t element_size;
  std::memcpy(&element_size, ptr_, 2);
  return std::string_view(ptr_ + 2, element_size);
}

log_entry::log_entry(log_line_id id) : id_(id) {}

}  // namespace nth
