#include "nth/debug/trace/trace.h"
#include "nth/debug/trace/internal/trace.h"

namespace nth {

void RegisterExpectationResultHandler(
    void (*handler)(debug::ExpectationResult const &)) {
  nth::debug::internal_trace::RegisterExpectationResultHandler(handler);
}

}  // namespace nth
