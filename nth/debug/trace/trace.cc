#include "nth/debug/trace/trace.h"

#include "nth/debug/trace/internal/trace.h"

namespace nth::debug {

void RegisterExpectationResultHandler(
    void (*handler)(ExpectationResult const &)) {
  internal_trace::RegisterExpectationResultHandler(handler);
}

}  // namespace nth::debug
