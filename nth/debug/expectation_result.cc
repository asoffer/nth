#include "nth/debug/expectation_result.h"

namespace nth::debug {
namespace {

using handler_type = void (*)(ExpectationResult const &);
using Registrar = internal_base::Registrar<struct Key, handler_type>;

}  // namespace

void RegisterExpectationResultHandler(handler_type handler) {
  Registrar::Register(handler);
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

internal_base::RegistrarImpl<void (*)(ExpectationResult const &)>::Range
RegisteredExpectationResultHandlers() {
  return Registrar::Registry();
}

}  // namespace nth::debug
