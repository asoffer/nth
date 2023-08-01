#ifndef NTH_COMMANDLINE_COMMANDLINE_H
#define NTH_COMMANDLINE_COMMANDLINE_H

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/io/printer.h"
#include "nth/process/exit_code.h"
#include "nth/strings/text.h"
#include "nth/utility/required.h"

namespace nth {

// Represents a flag passed on the commandline. A flag must have a "full name"
// so that it can be speficied with two leading dashes. For example, if a flag's
// full name is "my-amazing-flag", Then the flag can be specified on the
// commandline as `--my-amazing-flag`. Optionally, a flag may also provide a
// single-character "short name", so that it can be specified with a single dash
// and that character. For example if "my-amazing-flag" had the short name 'a',
// it could be specified on the commandline as `-a`.
//
// Flags may have associated values, which can be specified via an equals-sign,
// followed by the flags value. For example, `--my-amazing-flag=hello`, or
// `-a=hello`.
//
// In addition a collection of short flags may be specifed all together with a
// single dash. That is, `-abc` is equivalent to specifying `-a -b -c`.
struct Flag {
  // Represents the name for a flag, including the flag's full name and
  // optionally its short name.
  struct Name {
    Name(std::string full) : full_(std::move(full)) {}
    Name(std::string full, char short_name)
        : full_(std::move(full)), short_(short_name) {}
    Name(char short_name, std::string full)
        : full_(std::move(full)), short_(short_name) {}

    std::string_view full_name() const { return full_; }
    std::optional<char> short_name() const { return short_; }

   private:
    std::string full_;
    std::optional<char> short_;
  };

  // Represents the value bound to a flag.
  struct Value {
    Value(NTH_ATTRIBUTE(lifetimebound) Name const &name)
        : name_(&name), value_(std::nullopt) {}
    Value(NTH_ATTRIBUTE(lifetimebound) Name const &name, std::string_view value)
        : name_(&name), value_(value) {}

    std::string_view full_name() const { return name_->full_name(); }
    std::optional<char> short_name() const { return name_->short_name(); }
    std::optional<std::string_view> value() const { return value_; }

   private:
    Flag::Name const *name_;
    std::optional<std::string_view> value_;
  };

  Name name               = required;
  std::string description = required;
};

// A collection of flags and their bound values.
struct FlagValueSet {
  void insert(Flag::Value const &v);

  template <typename T>
  requires(std::is_same_v<T, std::string_view>) std::optional<T> get(
      std::string_view name)
  const { return get_impl(name); }

 private:
  std::optional<std::string_view> get_impl(std::string_view name) const;
  std::vector<Flag::Value> values_;
};

struct Command {
  // The name of the command being executed
  std::string name        = required;

  // A description of the command.
  std::string description = required;

  // A list of positional subcommands. This could be, for example the "status"
  // is "git status" or the "build" in "bazel build".
  std::vector<Command> subcommands;

  // A list of flags (specified either with full names as in "--full", or with
  // short names as in "-s").
  std::vector<Flag> flags;

  // The function to execute if this command is selected. If no function is
  // provided, then all subcommands must be executable.
  exit_code (*execute)(FlagValueSet,
                       std::span<std::string_view const>) = nullptr;
};

struct Usage {
  // The name of the program being executed. This is the name to be shown when
  // displaying usage information to the user. if No name is provided, the
  // zeroth argument (`argv[0]`) passed to the program will be used.
  std::optional<std::string> name = std::nullopt;

  // A high-level description of the what the program does.
  std::string description = required;

  // A list of positional commands. This could be, for example the "status" is
  // "git status" or the "build" in "bazel build".
  std::vector<Command> commands;
  std::vector<Flag> flags;

  // The function to execute. If no function is provided, then all commands must
  // be executable.
  exit_code (*execute)(FlagValueSet,
                       std::span<std::string_view const>) = nullptr;
};

// A command which displays information about how to use the program.
Command HelpCommand();

// The global variable `program_usage` must be defined for each binary to
// specify how commandline arguments are to be interpreted.
extern Usage const program_usage;

// Inovkes the `nth::program_usage` object with the given `arguments`. If
// parsing succeeds, the corresponding `execute` functions is invoked and its
// returned `exit_code` is returned. Otherwise `exit_code::usage` is returned.
exit_code InvokeCommandline(std::span<std::string_view const> arguments);

}  // namespace nth

#endif  // NTH_COMMANDLINE_COMMANDLINE_H
