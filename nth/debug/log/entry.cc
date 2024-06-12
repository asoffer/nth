#include "nth/debug/log/entry.h"

namespace nth {

log_entry::const_iterator& log_entry::const_iterator::operator++() {
  uint16_t element_size;
  std::memcpy(&element_size, ptr_ + 2, 2);
  ptr_ += element_size + 4;
  return *this;
}

std::string_view log_entry::const_iterator::operator*() const {
  uint16_t element_size;
  std::memcpy(&element_size, ptr_ + 2, 2);
  return std::string_view(ptr_ + 4, element_size);
}

log_entry::log_entry(log_line_id id) : id_(id) {}

size_t log_entry::open_subtree() {
  size_t size = data_.size();
  data_.append(std::string_view("\xff\xff\xff\xff", 4));
  return size;
}

void log_entry::attach_leaf(std::string_view s) {
  size_t subtree_start = open_subtree();
  size_t element_start = open_element();
  write(s);
  close_element(element_start);
  close_subtree(subtree_start);
}

size_t log_entry::open_element() { return data_.size(); }

void log_entry::close_element(size_t n) {
  uint16_t width;
  if (n < sizeof(width)) { std::abort(); }
  if (n >= data_.size()) { std::abort(); }
  if (data_.size() - n > std::numeric_limits<uint16_t>::max()) { std::abort(); }
  width = data_.size() - n;
  std::memcpy(&data_[n - sizeof(width)], &width, sizeof(width));
}

void log_entry::close_subtree(size_t n) {
  if (n >= data_.size()) { std::abort(); }
  if (data_.size() - n > std::numeric_limits<uint16_t>::max()) { std::abort(); }
  uint16_t width = data_.size() - n;
  std::memcpy(&data_[n], &n, sizeof(width));
}

}  // namespace nth
