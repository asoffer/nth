#include "nth/test/benchmark_result.h"

#include "absl/synchronization/mutex.h"
#include "nth/utility/no_destructor.h"

namespace nth::test {
namespace {

using handler_type = void (*)(BenchmarkResult const &);

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

auto &BenchmarkResultHandlers() {
  static NoDestructor<Ends> expectation_result_handlers;
  return *expectation_result_handlers;
}

}  // namespace

void RegisterBenchmarkResultHandler(handler_type handler) {
  BenchmarkResultHandlers().insert(handler);
}

BenchmarkResultHandlerRange::const_iterator &
BenchmarkResultHandlerRange::const_iterator::operator++() {
  --ptr_;
  if ((reinterpret_cast<uintptr_t>(ptr_) % alignof(HandlerNode)) == 0) {
    ptr_ = AsNodePtr(ptr_)->data_[0];
    if (ptr_) { ptr_ += 6; }
  }
  return *this;
}

BenchmarkResultHandlerRange::const_iterator
BenchmarkResultHandlerRange::const_iterator::operator++(int) {
  const_iterator copy = *this;
  ++*this;
  return copy;
}

BenchmarkResultHandlerRange::const_iterator::value_type
BenchmarkResultHandlerRange::const_iterator::operator*() const {
  return *AsHandlerPtr(ptr_);
}

BenchmarkResultHandlerRange::const_iterator::value_type const *
BenchmarkResultHandlerRange::const_iterator::operator->() const {
  return AsHandlerPtr(ptr_);
}

BenchmarkResultHandlerRange RegisteredBenchmarkResultHandlers() {
  uintptr_t const *ptr;
  {
    absl::MutexLock lock(&BenchmarkResultHandlers().mutex);
    HandlerNode const *node = BenchmarkResultHandlers().head;
    if (node->count() == 0) {
      ptr = nullptr;
    } else {
      ptr =
          reinterpret_cast<uintptr_t const *>(&node->data_[0]) + node->count();
    }
  }
  return BenchmarkResultHandlerRange(
      BenchmarkResultHandlerRange::const_iterator(ptr));
}

}  // namespace nth::test
