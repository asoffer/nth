#include "nth/debug/expectation_result.h"

#include "nth/base/indestructible.h"
#include "nth/registration/registrar.h"

namespace nth::debug {
namespace {

using handler_type = void (*)(ExpectationResult const &);
indestructible<registrar<handler_type>> registrar_;

}  // namespace

void RegisterExpectationResultHandler(handler_type handler) {
  registrar_->insert(handler);
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

registrar<void (*)(ExpectationResult const &)>::range_type
RegisteredExpectationResultHandlers() {
  return registrar_->registry();
}

}  // namespace nth::debug
