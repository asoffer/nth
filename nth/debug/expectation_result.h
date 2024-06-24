#ifndef NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
#define NTH_DEBUG_TRACE_EXPECTATION_RESULT_H

#include "nth/debug/source_location.h"
#include "nth/registration/registrar.h"

namespace nth {

// `expectation_result`:
//
// Represents the result of a computation intended as verification during a test
// or debug-check.
struct expectation_result {
  static expectation_result success(source_location location);
  static expectation_result failure(source_location location);

  [[nodiscard]] constexpr bool success() const { return success_; }

  [[nodiscard]] constexpr struct source_location source_location() const {
    return location_;
  }

 private:
  expectation_result(struct source_location location, bool success);

  struct source_location location_;
  bool success_;
};

// `register_expectation_result_handler`:
//
// Registers `handler` to be executed any time an expectation is evaluated (be
// that `NTH_REQUIRE`, `NTH_ENSURE`, `NTH_EXPECT`, or `NTH_ASSERT`). The
// execution order of handlers is unspecified and may be executed concurrently.
// Handlers cannot be un-registered.
void register_expectation_result_handler(
    void (*handler)(expectation_result const&));

// `registered_expectation_result_handlers`:
//
// Returns a view over the globally registered `expectation_result` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will view
// all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `expectation_resultHandlerRange` will not contain the newly registered
// handler.
registrar<void (*)(expectation_result const&)>::range_type
registered_expectation_result_handlers();

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
