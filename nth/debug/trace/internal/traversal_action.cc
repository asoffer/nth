#include "nth/debug/trace/internal/traversal_action.h"

namespace nth::debug::internal_trace {

void TraversalContext::Traverse(std::vector<TraversalAction> &&stack) {
  while (not stack.empty()) {
    auto action = stack.back();
    stack.pop_back();
    if (action.enter()) {
      Enter();
    } else if (action.expand()) {
      action.expand(stack);
    } else if (action.last()) {
      Last();
    } else if (action.exit()) {
      Exit();
    } else {
      SelfBeforeAction();
      action.act(*this);
      SelfAfterAction();
    }
  }
}

}  // namespace nth::debug::internal_trace
