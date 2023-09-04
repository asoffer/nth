#ifndef NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
#define NTH_DEBUG_TRACE_EXPECTATION_RESULT_H

#include "nth/debug/source_location.h"

namespace nth::debug {

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

}  // namespace nth::debug

#endif  // NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
