#include "nth/commandline/internal/invoke.h"

#include <span>
#include <string_view>
#include <vector>

#include "nth/commandline/internal/parse.h"
#include "nth/debug/log/log.h"

namespace nth::internal_commandline {
namespace {

exit_code InvokeCommandline(std::span<Command const> commands, Executor exec,
                            std::vector<std::vector<Flag> const *> &valid_flags,
                            std::span<std::string_view const> arguments) {
  FlagParsingState state(valid_flags);
  std::vector<std::string_view> positional_arguments;

  // A parsing error has already occurred.
  bool ok = true;

  size_t position = 0;
  while (true) {
    struct Incrementer {
      Incrementer(size_t &p) : p_(p) {}
      ~Incrementer() { ++p_; }

     private:
      size_t &p_;
    } increment_position{position};

    if (arguments.empty()) {
      if (not ok) { return exit_code::usage; }
      if (not exec) {
        NTH_LOG("No arguments provided.");
        return exit_code::usage;
      } else {
        return exec(std::move(state).flags(), positional_arguments);
      }
    }

    if (arguments[0].empty()) {
      arguments = arguments.subspan(1);
      continue;
    }

    std::string_view argument = arguments[0];
    if (argument[0] == '-') {
      ok &= state.ParseFlag(position, argument);
      arguments = arguments.subspan(1);
      continue;
    }

    auto iter =
        std::find_if(commands.begin(), commands.end(),
                     [&](auto const &cmd) { return cmd.name == arguments[0]; });
    if (iter == commands.end()) {
      positional_arguments.push_back(argument);
      arguments = arguments.subspan(1);
      for (std::string_view argument : arguments) {
        ++position;
        if (argument[0] == '-') {
          ok &= state.ParseFlag(position, argument);
        } else {
          positional_arguments.push_back(argument);
        }
      }

      if (exec) {
        if (not ok) { return exit_code::usage; }
        return exec(std::move(state).flags(), positional_arguments);
      } else {
        NTH_LOG("Invalid command.");
        return exit_code::usage;
      }
    }

    commands  = iter->subcommands;
    exec      = iter->execute;
    arguments = arguments.subspan(1);
    valid_flags.push_back(&iter->flags);
  }
}

}  // namespace

thread_local Usage const *current_usage;

exit_code InvokeCommandline(nth::Usage const &usage,
                            std::span<std::string_view const> arguments) {
  std::vector<std::vector<Flag> const *> valid_flags;
  valid_flags.push_back(&usage.flags);
  internal_commandline::current_usage = &usage;
  return InvokeCommandline(usage.commands, usage.execute, valid_flags,
                           arguments.subspan(1));
}

}  // namespace nth::internal_commandline
