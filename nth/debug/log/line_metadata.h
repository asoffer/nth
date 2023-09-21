#ifndef NTH_DEBUG_LOG_LINE_METADATA_H
#define NTH_DEBUG_LOG_LINE_METADATA_H

#include "nth/debug/source_location.h"

namespace nth {

// Metadata pertaining to the debug-log line including where it was encountered,
// invocation time, thread-id, etc.
struct LogLineMetadata {
  explicit LogLineMetadata(struct source_location loc)
      : source_location_(loc) {}

  // The location in source at which the log occurs.
  struct source_location source_location() const {
    return source_location_;
  }

  friend void NthPrint(auto& printer, auto& formatter,
                       LogLineMetadata const& metadata) {
    printer.write("\x1b[0;34m");
    formatter(printer, metadata.source_location().file_name());
    printer.write(" ");
    formatter(printer, metadata.source_location().function_name());
    printer.write(":");
    formatter(printer, metadata.source_location().line());
    printer.write("]\x1b[0m ");
  }

 private:
  struct source_location source_location_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_METADATA_H
