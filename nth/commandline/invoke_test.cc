#include "nth/commandline/internal/invoke.h"

#include "nth/commandline/commandline.h"
#include "nth/commandline/internal/rock_paper_scissors.h"
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

  count     = 0;
  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(count == 1);

  count     = 0;
  arguments = {"program", "more", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::usage);
  NTH_EXPECT(count == 0);

  count     = 0;
  arguments = {"program", "-f", "--flags", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::usage);
  NTH_EXPECT(count == 0);

  count     = 0;
  arguments = {"program", "-f", "--flags"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::usage);
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
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::usage);
  NTH_EXPECT(count == 0);

  count     = 0;
  arguments = {"program", "-f", "--flags", "arguments"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::usage);
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

NTH_TEST("commandline/typed-flags") {
  static std::vector<std::optional<int>> v;
  nth::Usage usage{
      .description = "description",
      .flags =
          {
              {
                  .name        = {"number", 'n'},
                  .type        = nth::type<int>,
                  .description = "",
              },
          },
      .execute =
          +[](FlagValueSet flags) {
            auto* p = flags.get<int>("number");
            if (p) {
              v.push_back(*p);
            } else {
              v.push_back(std::nullopt);
            }
            return exit_code::success;
          },
  };

  std::vector<std::string_view> arguments;

  v.clear();
  arguments = {"program", "--number=3"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(v == std::vector<std::optional<int>>{3});

  v.clear();
  arguments = {"program", "--number"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(v == std::vector<std::optional<int>>{});

  v.clear();
  arguments = {"program"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(v == std::vector<std::optional<int>>{std::nullopt});
}

NTH_TEST("commandline/typed-flags-type-change") {
  static std::vector<std::optional<int>> is;
  static std::vector<std::optional<bool>> bs;
  nth::Usage usage{
      .description = "description",
      .commands =
          {
              {
                  .name        = "run",
                  .description = "",
                  .flags =
                      {
                          {
                              .name        = {"value"},
                              .type        = nth::type<bool>,
                              .description = "",
                          },
                      },
                  .execute =
                      +[](FlagValueSet flags) {
                        auto* p = flags.get<bool>("value");
                        if (p) {
                          bs.push_back(*p);
                        } else {
                          bs.push_back(std::nullopt);
                        }
                        return exit_code::success;
                      },
              },
          },
      .flags =
          {
              {
                  .name        = {"value"},
                  .type        = nth::type<int>,
                  .description = "",
              },
          },
      .execute =
          +[](FlagValueSet flags) {
            auto* p = flags.get<int>("value");
            if (p) {
              is.push_back(*p);
            } else {
              is.push_back(std::nullopt);
            }
            return exit_code::success;
          },
  };

  std::vector<std::string_view> arguments;

  is.clear();
  bs.clear();
  arguments = {"program", "--value=3"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(is == std::vector<std::optional<int>>{3});
  NTH_EXPECT(bs == std::vector<std::optional<bool>>{});

  is.clear();
  bs.clear();
  arguments = {"program", "--value"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(is == std::vector<std::optional<int>>{});
  NTH_EXPECT(bs == std::vector<std::optional<bool>>{});

  is.clear();
  bs.clear();
  arguments = {"program", "run", "--value=true"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) == exit_code::success);
  NTH_EXPECT(is == std::vector<std::optional<int>>{});
  NTH_EXPECT(bs == std::vector<std::optional<bool>>{true});

  is.clear();
  bs.clear();
  arguments = {"program", "run", "--value"};
  NTH_EXPECT(InvokeCommandline(usage, arguments) != exit_code::success);
  NTH_EXPECT(is == std::vector<std::optional<int>>{});
  NTH_EXPECT(bs == std::vector<std::optional<bool>>{});
}

}  // namespace
}  // namespace nth::internal_commandline
