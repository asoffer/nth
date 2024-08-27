#include "nth/debug/log/entry.h"

#include <cstring>

namespace nth {

log_entry::builder::nth_write_result_type log_entry::builder::write(
    std::span<std::byte const> bytes) {
  entry_.data_.append(reinterpret_cast<char const *>(bytes.data()),
                      bytes.size());
  return nth_write_result_type(bytes.size());
}

log_entry::log_entry(size_t id) : id_(id) {}

}  // namespace nth
