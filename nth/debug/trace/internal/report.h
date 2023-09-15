#ifndef NTH_DEBUG_TRACE_INTERNAL_REPORT_H
#define NTH_DEBUG_TRACE_INTERNAL_REPORT_H

#include <cstdint>
#include <vector>

#include "nth/debug/trace/internal/traced_expr_base.h"
#include "nth/debug/trace/internal/traversal_action.h"

namespace nth::debug::internal_trace {

struct ReportActionStack {
  std::vector<TraversalAction> actions_;
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_REPORT_H
