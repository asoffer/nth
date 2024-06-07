#ifndef NTH_STRINGS_FORMAT_H
#define NTH_STRINGS_FORMAT_H

#include <string_view>

#include "nth/strings/interpolate/string.h"

namespace nth {

inline consteval auto format_string(auto const& formatter) {
  return interpolation_string(NthFormatString(formatter));
}

}  // namespace nth

#endif  // NTH_STRINGS_FORMAT_H
