#ifndef NTH_COMMANDLINE_INTERNAL_PARSE_H
#define NTH_COMMANDLINE_INTERNAL_PARSE_H

#include <string_view>
#include <utility>
#include <vector>

#include "nth/commandline/commandline.h"

namespace nth::internal_commandline {

struct FlagParsingState {
  explicit FlagParsingState(std::vector<std::vector<Flag> const *> &valid_flags)
      : valid_flags_(valid_flags) {}

  FlagValueSet &&flags() && { return std::move(flags_); }

  bool ParseFlag(size_t position, std::string_view argument);

 private:
  Flag const *find(char c);
  Flag const *find(std::string_view name);

  bool parse(char short_flag);
  bool parse(size_t position, std::string_view argument);

  std::vector<std::vector<Flag> const *> &valid_flags_;
  FlagValueSet flags_;
};

}  // namespace nth::internal_commandline

#endif  // NTH_COMMANDLINE_INTERNAL_PARSE_H
