#include "nth/debug/expectation_result.h"

#include "absl/synchronization/mutex.h"
#include "nth/utility/no_destructor.h"

namespace nth::debug {
namespace {

using handler_type = void (*)(ExpectationResult const &);

struct alignas(8 * alignof(uintptr_t)) HandlerNode {
  explicit HandlerNode(HandlerNode *next, uintptr_t count) : count_(count) {
    data_[0] = reinterpret_cast<uintptr_t *>(next);
  }

  void increment_count() { ++count_; }
  uintptr_t count() const { return count_; }

  uintptr_t *data_[7];
  uintptr_t count_;
};
static_assert(sizeof(HandlerNode) == 8 * sizeof(uintptr_t));

HandlerNode const *AsNodePtr(uintptr_t const *n) {
  return reinterpret_cast<HandlerNode const *>(n);
}

handler_type const *AsHandlerPtr(uintptr_t const *n) {
  return reinterpret_cast<handler_type const *>(n);
}

struct Ends {
  HandlerNode tail{nullptr, 0};
  HandlerNode *head = &tail;
  absl::Mutex mutex;

  void insert(handler_type h) {
    handler_type *location;
    {
      absl::MutexLock lock(&mutex);
      if (head->count() < 6) {
        head->increment_count();
      } else {
        head = new HandlerNode(head, 1);
      }
      location = reinterpret_cast<handler_type *>(head->data_) + head->count();
    }
    *location = h;
  }
};

auto &ExpectationResultHandlers() {
  static NoDestructor<Ends> expectation_result_handlers;
  return *expectation_result_handlers;
}

}  // namespace

void RegisterExpectationResultHandler(handler_type handler) {
  ExpectationResultHandlers().insert(handler);
}

ExpectationResult::ExpectationResult(nth::source_location location,
                                     bool success)
    : location_(location), success_(success) {}

ExpectationResult ExpectationResult::Success(nth::source_location location) {
  return ExpectationResult(location, true);
}

ExpectationResult ExpectationResult::Failure(nth::source_location location) {
  return ExpectationResult(location, false);
}

ExpectationResultHandlerRange::const_iterator &
ExpectationResultHandlerRange::const_iterator::operator++() {
  --ptr_;
  if ((reinterpret_cast<uintptr_t>(ptr_) % alignof(HandlerNode)) == 0) {
    ptr_ = AsNodePtr(ptr_)->data_[0];
    if (ptr_) { ptr_ += 6; }
  }
  return *this;
}

ExpectationResultHandlerRange::const_iterator
ExpectationResultHandlerRange::const_iterator::operator++(int) {
  const_iterator copy = *this;
  ++*this;
  return copy;
}

ExpectationResultHandlerRange::const_iterator::value_type
ExpectationResultHandlerRange::const_iterator::operator*() const {
  return *AsHandlerPtr(ptr_);
}

ExpectationResultHandlerRange::const_iterator::value_type const *
ExpectationResultHandlerRange::const_iterator::operator->() const {
  return AsHandlerPtr(ptr_);
}

ExpectationResultHandlerRange RegisteredExpectationResultHandlers() {
  uintptr_t const *ptr;
  {
    absl::MutexLock lock(&ExpectationResultHandlers().mutex);
    HandlerNode const *node = ExpectationResultHandlers().head;
    if (node->count() == 0) {
      ptr = nullptr;
    } else {
      ptr = reinterpret_cast<uintptr_t const *>(&node->data_[0]) + node->count();
    }
  }
  return ExpectationResultHandlerRange(
      ExpectationResultHandlerRange::const_iterator(ptr));
}

}  // namespace nth::debug
