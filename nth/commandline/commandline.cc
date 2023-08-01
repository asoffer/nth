#include "nth/commandline/commandline.h"

#include <algorithm>

#include "nth/commandline/internal/parse.h"
#include "nth/debug/debug.h"
#include "nth/debug/log/log.h"
#include "nth/io/file_printer.h"

namespace nth {
namespace internal_commandline {
namespace {

thread_local Usage const *current_usage;

exit_code InvokeCommandline(
    std::span<Command const> commands,
    exit_code (*exec)(FlagValueSet, std::span<std::string_view const>),
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
      if (not exec) {
        NTH_LOG((v.always), "No arguments provided.");
        return exit_code::usage;
      } else {
        return exec(std::move(state).flags(), arguments);
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
        return exec(std::move(state).flags(), arguments);
      } else {
        NTH_LOG((v.always), "Invalid command.");
        return exit_code::usage;
      }
    }

    commands  = iter->subcommands;
    exec      = iter->execute;
    arguments = arguments.subspan(1);
    valid_flags.push_back(&iter->flags);
  }
}

void WriteHelpMessage(Usage const &usage, nth::Printer auto &p) {
  p.write(usage.description);
  p.write("\n\n");
  size_t max_length =
      std::max_element(usage.commands.begin(), usage.commands.end(),
                       [](auto const &l, auto const &r) {
                         return l.name.size() < r.name.size();
                       })
          ->name.size();

  for (auto const &cmd : usage.commands) {
    p.write(max_length + 2 - cmd.name.size(), ' ');
    p.write(cmd.name);
    p.write("  ");
    std::string_view s = cmd.description;

    size_t d = 0;
    while (not s.empty()) {
      p.write(std::exchange(d, max_length + 4), ' ');
      p.write(GreedyLineBreak(s, 76 - max_length, text_encoding::ascii));
      p.write("\n");
    }
    p.write("\n");
  }
}

struct FlagComparator {
  bool operator()(Flag::Value const &lhs, std::string_view rhs) const {
    return lhs.full_name() < rhs;
  }

  bool operator()(Flag::Value const &lhs, Flag::Value const &rhs) const {
    return lhs.full_name() < rhs.full_name();
  }
}; 
constexpr FlagComparator compare_flag_values;

}  // namespace
}  // namespace internal_commandline

void FlagValueSet::insert(Flag::Value const &v) {
  auto iter = std::upper_bound(values_.begin(), values_.end(), v,
                               internal_commandline::compare_flag_values);
  values_.insert(iter, v);
}

std::optional<std::string_view> FlagValueSet::get_impl(
    std::string_view name) const {
  auto iter = std::lower_bound(values_.begin(), values_.end(), name,
                               internal_commandline::compare_flag_values);
  NTH_ASSERT(iter != values_.end());
  NTH_ASSERT(iter->full_name() == name);
  return iter->value();
}

Command HelpCommand() {
  return {.name        = "help",
          .description = "Prints a program usage information to the terminal.",
          .execute     = [](FlagValueSet, std::span<std::string_view const>) {
            internal_commandline::WriteHelpMessage(
                    *internal_commandline::current_usage, nth::stderr_printer);
            return exit_code::success;
          }};
}

exit_code InvokeCommandline(std::span<std::string_view const> arguments) {
  std::vector<std::vector<Flag> const *> valid_flags;
  valid_flags.push_back(&program_usage.flags);
  internal_commandline::current_usage = &program_usage;
  return internal_commandline::InvokeCommandline(
      program_usage.commands, program_usage.execute, valid_flags,
      arguments.subspan(1));
}

}  // namespace nth
