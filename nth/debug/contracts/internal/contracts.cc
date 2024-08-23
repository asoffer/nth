#include "nth/debug/contracts/internal/contracts.h"

#include <cstdlib>

namespace nth::internal_contracts {

NTH_ATTRIBUTE(weak)
void ensure_failed() { std::abort(); }
NTH_ATTRIBUTE(weak)
void require_failed() { std::abort(); }

NTH_ATTRIBUTE_TRY(inline_never)
void handle_contract_violation(contract const& c, any_formattable_ref afr) {
  for (auto handler : nth::registered_contract_violation_handlers()) {
    handler(nth::contract_violation(c, afr));
  }
}

}  // namespace nth::internal_contracts
