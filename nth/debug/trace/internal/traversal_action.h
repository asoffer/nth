#ifndef NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H
#define NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H

#include <cstdint>
#include <vector>

#include "nth/debug/trace/internal/formatter.h"
#include "nth/io/string_printer.h"

namespace nth::debug::internal_trace {

struct TraversalAction {
  using action_type = void (*)(void const *, formatter &,
                               bounded_string_printer &,
                               std::vector<TraversalAction> &);

  static TraversalAction Self(action_type act, void const *ptr) {
    return TraversalAction(act, reinterpret_cast<uintptr_t>(ptr));
  }

  static constexpr TraversalAction Enter() {
    return TraversalAction(nullptr, 0);
  }

  static constexpr TraversalAction Expand(action_type act, void const *ptr) {
    return TraversalAction(act, reinterpret_cast<uintptr_t>(ptr) + 1);
  }

  static constexpr TraversalAction Last() {
    return TraversalAction(nullptr, 2);
  }

  static constexpr TraversalAction Exit() {
    return TraversalAction(nullptr, 4);
  }

  bool enter() const { return data_ == 0; }
  bool expand() const { return (data_ & uintptr_t{1}) != 0; }
  bool last() const { return data_ == 2; }
  bool exit() const { return data_ == 4; }

  void act(formatter &f, bounded_string_printer &p,
           std::vector<TraversalAction> &stack) const {
    act_(reinterpret_cast<void const *>(data_ & ~uintptr_t{1}), f, p, stack);
  }

 private:
  explicit constexpr TraversalAction(action_type act, uintptr_t data)
      : act_(act), data_(data) {}
  action_type act_;
  uintptr_t data_;
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H
