#ifndef NTH_DEBUG_SOURCE_LOCATION_H
#define NTH_DEBUG_SOURCE_LOCATION_H

#include <string_view>

#include "nth/utility/unconstructible.h"

namespace nth {

// Represents a source location for the purposes of debugging. There are no
// guarantees the values of any 
struct source_location {
  source_location() = delete;

  // Returns a `source_location` reperesenting the location at which this
  // function call is invoked.
  static constexpr source_location current(
      unconstructible_except_by<source_location> = {},
      char const* file                           = __builtin_FILE(),
      char const* function                       = __builtin_FUNCTION(),
      int line                                   = __builtin_LINE()) {
    return source_location(file, function, line);
  }

  // Returns the name of the file corresponding to this source location.
  constexpr std::string_view file_name() const { return file_; }
  // Returns the name of the function corresponding to this source location.
  constexpr std::string_view function_name() const { return function_; }
  // Returns the line number corresponding to this source location.
  constexpr unsigned line() const { return line_; }

 private:
  explicit constexpr source_location(char const* file, char const* function,
                                     unsigned line)
      : file_(file), function_(function), line_(line) {}

  char const* file_;
  char const* function_;
  unsigned line_;
};

}  // namespace nth

#endif  // NTH_DEBUG_SOURCE_LOCATION_H
