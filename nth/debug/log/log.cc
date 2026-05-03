#include "nth/debug/log/log.h"

#include <string>
#include <string_view>

#include "nth/base/section.h"
#include "nth/strings/glob.h"

namespace nth {

void log_verbosity_off(std::string_view glob) {
  for (auto& log_line : nth::section<"nth_log_line">) {
    if (GlobMatches(glob, log_line.verbosity_path())) { log_line.disable(); }
  }
}

void log_verbosity_on(std::string_view glob, std::string_view config) {
  for (auto& log_line : nth::section<"nth_log_line">) {
    if (GlobMatches(glob, log_line.verbosity_path())) {
      log_line.enable(config);
    }
  }
}

void log_verbosity_if(std::string_view glob, std::string_view config) {
  for (auto& log_line : nth::section<"nth_log_line">) {
    if (GlobMatches(glob, log_line.verbosity_path())) {
      log_line.enable(config);
    } else {
      log_line.disable();
    }
  }
}

}  // namespace nth
