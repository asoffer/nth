#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

#include "nth/configuration/log.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/interpolate.h"

namespace nth {

// A particular invocation of an `NTH_LOG` statement along with encoded
// arguments.
struct log_entry {
  struct const_iterator {
    std::string_view operator*() const;

    friend bool operator==(const_iterator, const_iterator) = default;

    const_iterator& operator++();

    const_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }

   private:
    explicit const_iterator(char const* ptr) : ptr_(ptr) {}
    friend log_entry;
    char const* ptr_;
  };

  const_iterator component_begin() const {
    return const_iterator(data_.data());
  }
  const_iterator component_end() const {
    return const_iterator(data_.data() + data_.size());
  }

  log_line_id id() const { return id_; }

  template <interpolation_string>
  friend struct log_line;

  log_entry() = delete;
  explicit log_entry(log_line_id id);

  void attach_leaf(std::string_view);

  [[nodiscard]] size_t open_subtree();
  [[nodiscard]] size_t open_element();
  void write(std::string_view s) { data_.append(s); }
  std::string& data() { return data_; }
  void close_element(size_t start);
  void close_subtree(size_t start);

 private:
  void set_subtree_prefix(size_t index, uint16_t value);
  void set_entry_prefix(size_t index, uint16_t value);

  log_line_id id_;
  // Log data is stored structurally as a tree with all the data needed to
  // traverse it in a preorder traversal stored inline. Each entry is prefixed
  // with four bytes. The first two bytes indicate the the number of bytes in
  // the subtree rooted at that element (including all prefixes). The next two
  // bytes indicate the number of bytes in just that element (including the
  // length prefix). In this way, a leaf can be detected by determining that the
  // number of bytes in the subtree is two more than the number of bytes in the
  // element itself.
  std::string data_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
