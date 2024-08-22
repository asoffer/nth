#include "nth/debug/contracts/internal/contracts.h"

#include <cstdlib>

namespace nth::internal_contracts {

void ensure_failed() { std::abort(); }
void require_failed() { std::abort(); }

}  // namespace nth::internal_contracts
