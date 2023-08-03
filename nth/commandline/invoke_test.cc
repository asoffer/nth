#include "nth/commandline/internal/invoke.h"

#include "nth/commandline/commandline.h"
#include "nth/test/test.h"

nth::Usage const nth::program_usage = {.description = ""};

namespace nth::internal_commandline {
namespace {

NTH_TEST("commandline/no-flags-or-arguments") {
  static int count = 0;
  nth::Usage usage{
      .description = "description",
      .execute =
          +[](FlagValueSet) {
            ++count;
            return exit_code::success;
          },
  };

  std::vector<std::string_view> arguments;

  count = 0;
  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(count == 1);

  count = 0;
  arguments = {"program", "more", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(count == 0);

  count = 0;
  arguments = {"program", "-f", "--flags", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(count == 0);

  count = 0;
  arguments = {"program", "-f", "--flags"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(count == 0);
}

NTH_TEST("commandline/flags-without-arguments") {
  static int count = 0;
  nth::Usage usage{
      .description = "description",
      .flags =
          {
              {
                  .name        = {"flags"},
                  .description = "",
              },
              {
                  .name        = {"another-flag", 'f'},
                  .description = "",
              },
          },
      .execute =
          +[](FlagValueSet) {
            ++count;
            return exit_code::success;
          },
  };

  std::vector<std::string_view> arguments;

  count     = 0;
  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(count == 1);

  count     = 0;
  arguments = {"program", "more", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(count == 0);

  count     = 0;
  arguments = {"program", "-f", "--flags", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(count == 0);

  count     = 0;
  arguments = {"program", "-f", "--flags"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(count == 1);
}

NTH_TEST("commandline/flags-without-arguments") {
  static std::vector<std::vector<std::string_view>> calls;
  nth::Usage usage{
      .description = "description",
      .flags =
          {
              {
                  .name        = {"flags"},
                  .description = "",
              },
              {
                  .name        = {"another-flag", 'f'},
                  .description = "",
              },
          },
      .execute =
          +[](FlagValueSet, std::span<std::string_view const> arguments) {
            calls.emplace_back(arguments.begin(), arguments.end());
            return exit_code::success;
          },
  };

  std::vector<std::string_view> arguments;

  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{{}});

  calls.clear();
  arguments = {"program", "more", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"more", "arguments"}});

  calls.clear();
  arguments = {"program", "-f", "--flags", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "-f", "--flags"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{{}});
}

NTH_TEST("commandline/nested-commands") {
  static std::vector<std::vector<std::string_view>> calls;
  nth::Usage usage{
      .description = "description",
      .commands =
          {
              {
                  .name        = "run",
                  .description = "description",
                  .execute =
                      +[](FlagValueSet,
                          std::span<std::string_view const> arguments) {
                        calls.emplace_back(arguments.begin(), arguments.end());
                        return exit_code::success;
                      },
              },
          },
      .flags =
          {
              {
                  .name        = {"flags"},
                  .description = "",
              },
              {
                  .name        = {"another-flag", 'f'},
                  .description = "",
              },
          },
  };

  std::vector<std::string_view> arguments;

  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "run"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{{}});

  calls.clear();
  arguments = {"program", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "run", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "-f"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "-f", "run"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{{}});

  calls.clear();
  arguments = {"program", "-f", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "-f", "run", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "run", "-f", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "run", "arguments", "-f"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});
}

NTH_TEST("commandline/nested-flags") {
  static std::vector<std::vector<std::string_view>> calls;
  nth::Usage usage{
      .description = "description",
      .commands =
          {
              {
                  .name        = "run",
                  .description = "description",
                  .flags =
                      {
                          {
                              .name        = {"flags"},
                              .description = "",
                          },
                          {
                              .name        = {"another-flag", 'f'},
                              .description = "",
                          },
                      },
                  .execute =
                      +[](FlagValueSet,
                          std::span<std::string_view const> arguments) {
                        calls.emplace_back(arguments.begin(), arguments.end());
                        return exit_code::success;
                      },
              },
          },
  };

  std::vector<std::string_view> arguments;

  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "run"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{{}});

  calls.clear();
  arguments = {"program", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "run", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "-f"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "-f", "run"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "-f", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "-f", "run", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(calls == std::vector<std::vector<std::string_view>>{});

  calls.clear();
  arguments = {"program", "run", "-f", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});

  calls.clear();
  arguments = {"program", "run", "arguments", "-f"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(calls ==
             std::vector<std::vector<std::string_view>>{{"arguments"}});
}

enum class Throw { Rock, Paper, Scissors };
template <typename ParserErrorReporter>
bool NthCommandlineParse(std::string_view s, Throw &t,
                         ParserErrorReporter reporter) {
  if (s == "rock") {
    t = Throw::Rock;
    return true;
  } else if (s == "paper") {
    t = Throw::Paper;
    return true;
  } else if (s == "scissors") {
    t = Throw::Scissors;
    return true;
  } else {
    reporter("Argument must be one of 'rock', 'paper', or 'scissors'.");
    return false;
  }
}

NTH_TEST("commandline/parsing") {
  static std::vector<Throw> throws;
  nth::Usage usage{
      .description = "description",
      .commands =
          {
              {
                  .name        = "throw",
                  .description = "description",
                  .execute =
                      +[](FlagValueSet, Throw t) {
                        throws.push_back(t);
                        return exit_code::success;
                      },
              },
          },
  };

  std::vector<std::string_view> arguments;

  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<Throw>{});

  throws.clear();
  arguments = {"program", "throw"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<Throw>{});

  throws.clear();
  arguments = {"program", "rock"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<Throw>{});

  throws.clear();
  arguments = {"program", "throw", "rock"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(throws == std::vector<Throw>{Throw::Rock});

  throws.clear();
  arguments = {"program", "throw", "rock", "paper"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<Throw>{});
}

NTH_TEST("commandline/parsing-multiple-arguments") {
  static std::vector<std::pair<Throw, Throw>> throws;
  nth::Usage usage{
      .description = "description",
      .commands =
          {
              {
                  .name        = "throw",
                  .description = "description",
                  .execute =
                      +[](FlagValueSet, Throw l, Throw r) {
                        throws.emplace_back(l, r);
                        return exit_code::success;
                      },
              },
          },
  };

  std::vector<std::string_view> arguments;

  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<std::pair<Throw, Throw>>{});

  throws.clear();
  arguments = {"program", "throw"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<std::pair<Throw, Throw>>{});

  throws.clear();
  arguments = {"program", "rock"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<std::pair<Throw, Throw>>{});

  throws.clear();
  arguments = {"program", "throw", "rock"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(throws == std::vector<std::pair<Throw, Throw>>{});

  throws.clear();
  arguments = {"program", "throw", "rock", "paper"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(throws == std::vector{std::pair{Throw::Rock, Throw::Paper}});
}

}  // namespace
}  // namespace nth::internal_commandline
