#include "nth/debug/trace/expectation_result.h"

namespace nth::debug {

ExpectationResult::ExpectationResult(nth::source_location location,
                                     bool success)
    : location_(location), success_(success) {}

ExpectationResult ExpectationResult::Success(nth::source_location location) {
  return ExpectationResult(location, true);
}

ExpectationResult ExpectationResult::Failure(nth::source_location location) {
  return ExpectationResult(location, false);
}

}  // namespace nth::debug
