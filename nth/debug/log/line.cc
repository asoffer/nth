#include "nth/debug/log/line.h"

namespace nth {

LogLine const LogLine::stub_("", source_location::current());
std::atomic<LogLine const*> LogLine::head_{&LogLine::stub_};

LogLine::LogLine(std::string_view interpolation_string,
                 struct source_location location)
    : interpolation_string_(interpolation_string), source_location_(location) {
  LogLine const* head = nullptr;
  do {
    head  = head_.load(std::memory_order::relaxed);
    next_ = head;
    id_   = head->id_ + 1;
  } while (not head_.compare_exchange_weak(
      head, this, std::memory_order::release, std::memory_order::relaxed));
}

}  // namespace nth
