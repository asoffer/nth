#ifndef NTH_COMMANDLINE_INTERNAL_INVOKE_H
#define NTH_COMMANDLINE_INTERNAL_INVOKE_H

#include <span>
#include <string_view>

#include "nth/commandline/commandline.h"
#include "nth/process/exit_code.h"

namespace nth:: internal_commandline {

exit_code InvokeCommandline(nth::Usage const &usage,
                            std::span<std::string_view const> arguments);

}  // namespace nth::internal_commandline

#endif  // NTH_COMMANDLINE_INTERNAL_INVOKE_H
