#ifndef NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H
#define NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H

#include "nth/debug/source_location.h"
#include "nth/registration/registrar.h"

namespace nth {

// `contract_violation`:
//
// Represents a failing result of a computation intended as verification during
// a test or debug-check.
struct contract_violation {
  static contract_violation failure(source_location location);

  // TODO: Remove
  [[nodiscard]] constexpr bool success() const { return false; }

  [[nodiscard]] constexpr struct source_location source_location() const {
    return location_;
  }

 private:
  contract_violation(struct source_location location, bool success);

  struct source_location location_;
};

// `register_contract_violation_handler`:
//
// Registers `handler` to be executed any time an expectation is evaluated (be
// that `NTH_REQUIRE`, `NTH_ENSURE`, `NTH_EXPECT`, or `NTH_ASSERT`). The
// execution order of handlers is unspecified and may be executed
// concurrently. Handlers cannot be un-registered.
void register_contract_violation_handler(
    void (*handler)(contract_violation const&));

// `registered_contract_violation_handlers`:
//
// Returns a view over the globally registered `contract_violation` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will
// view all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `contract_violationHandlerRange` will not contain the newly registered
// handler.
registrar<void (*)(contract_violation const&)>::range_type
registered_contract_violation_handlers();

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H
