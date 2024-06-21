#include "nth/debug/log/log.h"

#include <string_view>

#include "nth/base/section.h"
#include "nth/strings/glob.h"

namespace nth {

void log_verbosity_off(std::string_view glob) {
  for (auto &log_line : nth::section<"nth_log_line">) {
    if (GlobMatches(glob, log_line.verbosity_path())) { log_line.disable(); }
  }
}

void log_verbosity_on(std::string_view glob) {
  for (auto &log_line : nth::section<"nth_log_line">) {
    if (GlobMatches(glob, log_line.verbosity_path())) { log_line.enable(); }
  }
}

void log_verbosity_if(std::string_view glob) {
  for (auto &log_line : nth::section<"nth_log_line">) {
    log_line.enable(GlobMatches(glob, log_line.verbosity_path()));
  }
}

}  // namespace nth
