#include "nth/commandline/commandline.h"

#include <algorithm>

#include "nth/commandline/internal/invoke.h"
#include "nth/commandline/internal/parse.h"
#include "nth/debug/debug.h"
#include "nth/io/printer.h"
#include "nth/io/file_printer.h"

namespace nth {
namespace internal_commandline {
namespace {

void WriteHelpMessage(Usage const &usage, nth::io::printer_type auto &p) {
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
    return lhs.flag().name.full_name() < rhs;
  }

  bool operator()(Flag::Value const &lhs, Flag::Value const &rhs) const {
    return lhs.flag().name.full_name() < rhs.flag().name.full_name();
  }
}; 
constexpr FlagComparator compare_flag_values;

}  // namespace

extern thread_local Usage const *current_usage;

}  // namespace internal_commandline

void FlagValueSet::insert(Flag::Value const &v) {
  auto iter = std::upper_bound(values_.begin(), values_.end(), v,
                               internal_commandline::compare_flag_values);
  values_.insert(iter, v);
}

Flag::Value const *FlagValueSet::get_impl(std::string_view name) const {
  auto iter = std::lower_bound(values_.begin(), values_.end(), name,
                               internal_commandline::compare_flag_values);
  if (iter == values_.end()) { return nullptr; }
  if (iter->flag().name.full_name() != name) { return nullptr; }
  return &*iter;
}

Command HelpCommand() {
  return {.name        = "help",
          .description = "Prints a program usage information to the terminal.",
          .execute     = +[](FlagValueSet, std::span<std::string_view const>) {
            internal_commandline::WriteHelpMessage(
                    *internal_commandline::current_usage, nth::stderr_printer);
            return exit_code::success;
          }};
}

exit_code InvokeCommandline(std::span<std::string_view const> arguments) {
  return internal_commandline::InvokeCommandline(program_usage, arguments);
}

}  // namespace nth
