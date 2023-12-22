#ifndef NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
#define NTH_DEBUG_TRACE_EXPECTATION_RESULT_H

#include <cstdint>

#include "nth/base/internal/global_registry.h"
#include "nth/debug/source_location.h"

namespace nth::debug {

// `ExpectationResult`:
//
// Represents the result of a computation intended as verification during a test
// or debug-check.
struct ExpectationResult {
  static ExpectationResult Success(nth::source_location location);
  static ExpectationResult Failure(nth::source_location location);

  bool success() const { return success_; }

  nth::source_location source_location() const { return location_; }

 private:
  ExpectationResult(nth::source_location location, bool success);

  nth::source_location location_;
  bool success_;
};

// `RegisterExpectationResultHandler`:
//
// Registers `handler` to be executed any time an expectation is evaluated (be
// that `NTH_REQUIRE`, `NTH_ENSURE`, `NTH_EXPECT`, or `NTH_ASSERT`). The
// execution order of handlers is unspecified and may be executed concurrently.
// Handlers cannot be un-registered.
void RegisterExpectationResultHandler(
    void (*handler)(ExpectationResult const&));

// `RegisteredExpectationResultHandlers`:
//
// Returns a view over the globally registered `ExpectationResult` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will view
// all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `ExpectationResultHandlerRange` will not contain the newly registered
// handler.
internal_base::RegistrarImpl<void (*)(ExpectationResult const&)>::Range
RegisteredExpectationResultHandlers();

}  // namespace nth::debug

#endif  // NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
