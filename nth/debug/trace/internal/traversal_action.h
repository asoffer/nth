#ifndef NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H
#define NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "nth/configuration/trace.h"
#include "nth/io/string_printer.h"

namespace nth::debug::internal_trace {

struct TraversalContext;

// Represents an action to take when traversing a traced object.
struct TraversalAction {
  using expand_type = void (*)(void const *, std::vector<TraversalAction> &);

  // Represents an action indicating processing `ptr` by applying `act`.
  static TraversalAction Self(void (*act)(void const *, TraversalContext &),
                              void const *ptr) {
    return TraversalAction(reinterpret_cast<void (*)()>(act),
                           reinterpret_cast<uintptr_t>(ptr));
  }

  // Represents an action indicating entering a subtree.
  static TraversalAction Enter() {
    return TraversalAction(nullptr, uintptr_t{0});
  }

  // Represents an action indicating expanding a subtree.
  static TraversalAction Expand(expand_type expand, void const *ptr) {
    return TraversalAction(expand, ptr);
  }

  // Represents an action notifying the traversal that the next entry (which may
  // possibly have a subtree) is the last entry among the children of the
  // current parent.
  static TraversalAction Last() { return TraversalAction(nullptr, 2); }

  // Reperesents an action indicating finishing processing of a given subtree.
  static TraversalAction Exit() { return TraversalAction(nullptr, 4); }

  bool enter() const { return data_ == 0; }
  bool expand() const { return (data_ & uintptr_t{1}) != 0; }
  bool last() const { return data_ == 2; }
  bool exit() const { return data_ == 4; }

  void expand(std::vector<TraversalAction> &stack) const {
    expand_(reinterpret_cast<void const *>(data_ & ~uintptr_t{1}), stack);
  }

  void act(TraversalContext &context) const {
    reinterpret_cast<void (*)(void const *, TraversalContext &)>(act_)(
        reinterpret_cast<void const *>(data_ & ~uintptr_t{1}), context);
  }

 private:
  explicit TraversalAction(expand_type expand, void const *ptr)
      : expand_(expand), data_(reinterpret_cast<uintptr_t>(ptr) + 1) {}
  explicit TraversalAction(void (*act)(), uintptr_t data)
      : act_(act), data_(data) {}
  union {
    void (*act_)();
    expand_type expand_;
  };
  uintptr_t data_;
};

struct TraversalContext {
  explicit TraversalContext(bounded_string_printer &printer)
      : printer_(printer) {}

  virtual void Enter()                = 0;
  virtual void Last()                 = 0;
  virtual void Exit()                 = 0;
  virtual void SelfBeforeAction()     = 0;
  virtual void SelfAfterAction()      = 0;

  void Traverse(std::vector<TraversalAction> &&stack);

  void write(auto const &v) { return formatter_(printer_, v); }

 protected:
  bounded_string_printer &printer() { return printer_; }

 private:
  bounded_string_printer &printer_;
  nth::config::trace_formatter formatter_;
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRAVERSAL_ACTION_H
