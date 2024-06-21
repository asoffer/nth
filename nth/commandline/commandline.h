#ifndef NTH_COMMANDLINE_COMMANDLINE_H
#define NTH_COMMANDLINE_COMMANDLINE_H

#include <algorithm>
#include <any>
#include <charconv>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

#include "nth/base/attributes.h"
#include "nth/debug/debug.h"
#include "nth/meta/type.h"
#include "nth/process/exit_code.h"
#include "nth/strings/text.h"
#include "nth/utility/required.h"

namespace nth {
namespace internal_commandline {
struct FlagParsingState;
}  // namespace internal_commandline

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
    Value(NTH_ATTRIBUTE(lifetimebound) Flag const &flag) : flag_(&flag) {}
    Value(NTH_ATTRIBUTE(lifetimebound) Flag const &flag, std::string_view value)
        : flag_(&flag) {
      flag_->type.store(value, value_);
    }

    Flag const &flag() const { return *flag_; }

    template <typename T>
    T const *value() const {
      return std::any_cast<T>(value_);
    }

   private:
    friend struct FlagValueSet;

    Flag const *flag_;
    std::any value_;
  };

  struct Type {
    Type(nth::Type auto t) requires(t != nth::type<void>);
    Type(nth::Type auto t) requires(t == nth::type<void>);

    bool store(std::string_view s, std::any &a) const {
      if (type_ == nth::type<void>) {
        NTH_LOG(
            "Attempting to store a value in a flag that does not have a "
            "value.");
        return false;
      }
      return store_(s, a);
    }

   private:
    friend struct FlagValueSet;
    friend internal_commandline::FlagParsingState;
    type_id type_;
    std::function<bool(std::string_view, std::any &)> store_;
  };

  Name name               = required;
  Type type               = nth::type<void>;
  std::string description = required;
};

// A collection of flags and their bound values.
struct FlagValueSet {
  void insert(Flag::Value const &v);

  // Returns a pointer to an object of type `T` passed in with the flag whose
  // full name is `name`, or a null pointer if the flag was not provided on the
  // commandline. If the type `T` does not match the type specified for the
  // flag, program execution will be aborted.
  template <typename T>
  T const *try_get(std::string_view name) const {
    auto *f = get_impl(name);
    if (not f) { return nullptr; }
    NTH_REQUIRE(f->flag().type.type_ == nth::type<std::remove_cvref_t<T>>);
    return std::addressof(
        std::any_cast<std::remove_cvref_t<T> const &>(f->value_));
  }

  template <typename T>
  T const &get(std::string_view name) const {
    T const *ptr = try_get<T>(name);
    NTH_REQUIRE(ptr != nullptr);
    return *ptr;
  }

 private:
  Flag::Value const *get_impl(std::string_view name) const;

  std::vector<Flag::Value> values_;
};

template <typename T, typename R>
requires(requires(std::string_view s, T &t, R r) {
  { NthCommandlineParse(s, t, r) } -> std::same_as<bool>;
}) bool ParseCommandlineArgument(std::string_view s, T &t, R r) {
  return NthCommandlineParse(s, t, r);
}

template <typename R>
bool NthCommandlineParse(std::string_view s, std::integral auto &n, R r) {
  auto [ptr, ec] = std::from_chars(s.begin(), s.end(), n);
  if (ptr != s.end()) {
    r("Failed to parse entirety of flag.");
    return false;
  } else if (ec != std::errc{}) {
    r("Failed to parse.");
    return false;
  }

  return true;
}

bool NthCommandlineParse(std::string_view s, std::string &out, auto) {
  out = s;
  return true;
}

template <typename R>
bool NthCommandlineParse(std::string_view s, bool &b, R r) {
  if (s == "true") {
    b = true;
    return true;
  } else if (s == "false") {
    b = false;
    return true;
  } else {
    r("Failed to parse bool.");
    return false;
  }
}

struct Executor {
 private:
  friend Flag::Type;
  struct ParseErrorReporter {
    void operator()(std::string_view message) { NTH_LOG("{}") <<= {message}; }
  };

 public:
  Executor() : exec_(nullptr) {}

  template <typename... Ts>
  Executor(exit_code (*f)(FlagValueSet, Ts...))
      : exec_([f](FlagValueSet flags,
                  std::span<std::string_view const> arguments) -> exit_code {
          if (arguments.size() != sizeof...(Ts)) {
            NTH_LOG(
                "Expected exactly {} commandline arguments, but {} were "
                "provided.") <<= {sizeof...(Ts), arguments.size()};
            return exit_code::usage;
          }

          ParseErrorReporter reporter;

          std::tuple<std::decay_t<Ts>...> ts;
          return std::apply(
              [&, i = size_t{0}](auto &...ts) mutable -> exit_code {
                std::array<bool, sizeof...(Ts)> parses = {
                    nth::ParseCommandlineArgument(arguments[i++], ts,
                                                  reporter)...};
                if (std::all_of(parses.begin(), parses.end(),
                                [](bool b) { return b; })) {
                  return f(std::move(flags), ts...);
                } else {
                  return exit_code::usage;
                }
              },
              ts);
        }) {}

  template <typename... Ts>
  Executor(exit_code (*f)(FlagValueSet, Ts...,
                          std::span<std::string_view const>))
      : exec_([f](FlagValueSet flags,
                  std::span<std::string_view const> arguments) -> exit_code {
          ParseErrorReporter reporter;

          std::tuple<std::decay_t<Ts>...> ts;
          return std::apply(
              [&, i = size_t{0}](auto &...ts) mutable -> exit_code {
                std::array<bool, sizeof...(Ts)> parses = {
                    nth::ParseCommandlineArgument(arguments[i++], ts,
                                                  reporter)...};
                if (std::all_of(parses.begin(), parses.end(),
                                [](bool b) { return b; })) {
                  return f(std::move(flags), ts...,
                           arguments.subspan(sizeof...(Ts)));
                } else {
                  return exit_code::usage;
                }
              },
              ts);
        }) {}

  explicit operator bool() const { return static_cast<bool>(exec_); }

  exit_code operator()(FlagValueSet flags,
                       std::span<std::string_view const> arguments) {
    return exec_(std::move(flags), arguments);
  }

 private:
  std::function<exit_code(FlagValueSet, std::span<std::string_view const>)>
      exec_;
};

struct Command {
  // The name of the command being executed
  std::string name = required;

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
  Executor execute;
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
  Executor execute;
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

Flag::Type::Type(nth::Type auto t) requires(t != nth::type<void>)
    : type_(t), store_([&](std::string_view s, std::any &a) {
        nth::type_t<t> object;
        Executor::ParseErrorReporter r;
        if (nth::ParseCommandlineArgument(s, object, r)) {
          a = std::move(object);
          return true;
        } else {
          NTH_LOG("Failed to parse flag with value \"{}\"") <<= {s};
          return false;
        }
      }) {}

Flag::Type::Type(nth::Type auto t) requires(t == nth::type<void>) : type_(t) {}

}  // namespace nth

#endif  // NTH_COMMANDLINE_COMMANDLINE_H
