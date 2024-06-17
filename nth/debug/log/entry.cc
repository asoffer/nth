#include "nth/debug/log/entry.h"

#include <cstring>

namespace nth {

void log_entry::builder::write(std::string_view s) { entry_.data_.append(s); }

log_entry::log_entry(log_line_id id) : id_(id) {}

}  // namespace nth
