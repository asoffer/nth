#include "nth/debug/log/line.h"

namespace nth {

log_line_base const log_line_base::stub_;
std::atomic<log_line_base const*> log_line_base::head_{&log_line_base::stub_};

log_line_base::log_line_base() : metadata_(nth::source_location::current()) {
  log_line_base const* head = nullptr;
  do {
    head = head_.load(std::memory_order::relaxed);
    id_  = head->id_ + 1;
  } while (not head_.compare_exchange_weak(
      head, this, std::memory_order::release, std::memory_order::relaxed));
}

}  // namespace nth
