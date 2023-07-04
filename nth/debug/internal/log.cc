#include "nth/debug/internal/log.h"

#include <atomic>

namespace nth::internal_log {
namespace {

LogLineBase stub;

}  // namespace

LogLineBase::LogLineBase(source_location location)
    : source_location_(location) {
  LogLineBase const* head = nullptr;
  do {
    head   = head_.load(std::memory_order::relaxed);
    next_  = head;
    index_ = head->index_ + 1;
  } while (not head_.compare_exchange_weak(
      head, this, std::memory_order::release, std::memory_order::relaxed));
}

std::atomic<LogLineBase const*> LogLineBase::head_{&stub};

}  // namespace nth::internal_log
