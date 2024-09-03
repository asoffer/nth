#include "nth/debug/contracts/contracts.h"

#include <string_view>

#include "nth/base/section.h"
#include "nth/strings/glob.h"

namespace nth {

void contract_check_off(std::string_view glob) {
  for (auto &enabler : nth::section<"nth_contract">) {
    if (GlobMatches(glob, enabler.mode_path())) { enabler.disable(); }
  }
}

void contract_check_on(std::string_view glob) {
  for (auto &enabler : nth::section<"nth_contract">) {
    if (GlobMatches(glob, enabler.mode_path())) { enabler.enable(); }
  }
}

void contract_check_if(std::string_view glob) {
  for (auto &enabler : nth::section<"nth_contract">) {
    enabler.enable(GlobMatches(glob, enabler.mode_path()));
  }
}
}  // namespace nth
