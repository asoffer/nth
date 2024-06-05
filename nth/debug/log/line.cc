#include "nth/debug/log/line.h"

namespace nth {

log_line const log_line::stub_("", source_location::current());
std::atomic<log_line const*> log_line::head_{&log_line::stub_};

log_line::log_line(std::string_view interpolation_string,
                   struct source_location location)
    : interpolation_string_(interpolation_string), metadata_(location) {
  log_line const* head = nullptr;
  do {
    head = head_.load(std::memory_order::relaxed);
    id_  = head->id_ + 1;
  } while (not head_.compare_exchange_weak(
      head, this, std::memory_order::release, std::memory_order::relaxed));
}

}  // namespace nth
