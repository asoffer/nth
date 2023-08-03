#include "nth/commandline/internal/parse.h"

#include "nth/commandline/commandline.h"
#include "nth/commandline/internal/rock_paper_scissors.h"
#include "nth/test/test.h"

nth::Usage const nth::program_usage = {.description = ""};

namespace nth::internal_commandline {
namespace {

NTH_TEST("commandline/no-valid-flags") {
  std::vector<std::vector<Flag> const *> valid_flags;
  FlagParsingState state(valid_flags);
  NTH_EXPECT(not state.ParseFlag(0, ""));
  NTH_EXPECT(not state.ParseFlag(0, "-"));
  NTH_EXPECT(not state.ParseFlag(0, "-x"));
  NTH_EXPECT(not state.ParseFlag(0, "-y"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy"));
  NTH_EXPECT(not state.ParseFlag(0, "--blah"));
  NTH_EXPECT(not state.ParseFlag(0, "--"));
  NTH_EXPECT(not state.ParseFlag(0, "--x"));
  NTH_EXPECT(not state.ParseFlag(0, "---"));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz="));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-x="));
  NTH_EXPECT(not state.ParseFlag(0, "-x=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy=abc"));
}

NTH_TEST("commandline/full-name") {
  std::vector<Flag> fs = {
      {
          .name        = {"flag"},
          .description = "",
      },
  };
  std::vector<std::vector<Flag> const *> valid_flags = {&fs};
  FlagParsingState state(valid_flags);
  NTH_EXPECT(not state.ParseFlag(0, ""));
  NTH_EXPECT(not state.ParseFlag(0, "-"));
  NTH_EXPECT(not state.ParseFlag(0, "-x"));
  NTH_EXPECT(not state.ParseFlag(0, "-y"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy"));
  NTH_EXPECT(not state.ParseFlag(0, "--blah"));
  NTH_EXPECT(not state.ParseFlag(0, "--"));
  NTH_EXPECT(not state.ParseFlag(0, "--x"));
  NTH_EXPECT(not state.ParseFlag(0, "---"));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz="));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-x="));
  NTH_EXPECT(not state.ParseFlag(0, "-x=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-flag"));
  NTH_EXPECT(state.ParseFlag(0, "--flag"));
  NTH_EXPECT(state.ParseFlag(0, "--flag="));
  NTH_EXPECT(state.ParseFlag(0, "--flag=xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=-xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--xyz"));
}

NTH_INVOKE_TEST("commandline/short-name") {
  // Test construction with short and full flags in both orders.
  co_yield Flag{
      .name        = {"flag", 'f'},
      .description = "",
  };
  co_yield Flag{
      .name        = {'f', "flag"},
      .description = "",
  };
}
NTH_TEST("commandline/short-name", Flag f) {
  std::vector<Flag> fs;
  fs.push_back(std::move(f));

  std::vector<std::vector<Flag> const *> valid_flags = {&fs};
  FlagParsingState state(valid_flags);
  NTH_EXPECT(not state.ParseFlag(0, ""));
  NTH_EXPECT(not state.ParseFlag(0, "-"));
  NTH_EXPECT(not state.ParseFlag(0, "-x"));
  NTH_EXPECT(not state.ParseFlag(0, "-y"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy"));
  NTH_EXPECT(not state.ParseFlag(0, "--blah"));
  NTH_EXPECT(not state.ParseFlag(0, "--"));
  NTH_EXPECT(not state.ParseFlag(0, "--x"));
  NTH_EXPECT(not state.ParseFlag(0, "---"));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz="));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-x="));
  NTH_EXPECT(not state.ParseFlag(0, "-x=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy=abc"));
  NTH_EXPECT(state.ParseFlag(0, "--flag"));
  NTH_EXPECT(state.ParseFlag(0, "--flag="));
  NTH_EXPECT(state.ParseFlag(0, "--flag=xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=-xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--xyz"));
  NTH_EXPECT(not state.ParseFlag(0, "-flag"));
  NTH_EXPECT(not state.ParseFlag(0, "f"));
  NTH_EXPECT(not state.ParseFlag(0, "--f"));
  NTH_EXPECT(not state.ParseFlag(0, "--f="));
  NTH_EXPECT(not state.ParseFlag(0, "--f=x"));
  NTH_EXPECT(state.ParseFlag(0, "-f"));
  NTH_EXPECT(not state.ParseFlag(0, "-f="));
  NTH_EXPECT(not state.ParseFlag(0, "-f=x"));
  NTH_EXPECT(not state.ParseFlag(0, "-f=-f"));
}

NTH_TEST("commandline/layered") {
  std::vector<Flag> layer0{
      {
          .name        = {'f', "flag"},
          .description = "",
      },
      {
          .name        = {'a', "all"},
          .description = "",
      },
  };
  std::vector<Flag> layer1{{
      .name        = {'f', "flag"},
      .description = "",
  },};

  std::vector<std::vector<Flag> const *> valid_flags = {&layer0, &layer1};
  FlagParsingState state(valid_flags);
  NTH_EXPECT(not state.ParseFlag(0, ""));
  NTH_EXPECT(not state.ParseFlag(0, "-"));
  NTH_EXPECT(not state.ParseFlag(0, "-x"));
  NTH_EXPECT(not state.ParseFlag(0, "-y"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy"));
  NTH_EXPECT(not state.ParseFlag(0, "--blah"));
  NTH_EXPECT(not state.ParseFlag(0, "--"));
  NTH_EXPECT(not state.ParseFlag(0, "--x"));
  NTH_EXPECT(not state.ParseFlag(0, "---"));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz="));
  NTH_EXPECT(not state.ParseFlag(0, "--xyz=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-x="));
  NTH_EXPECT(not state.ParseFlag(0, "-x=abc"));
  NTH_EXPECT(not state.ParseFlag(0, "-xy=abc"));
  NTH_EXPECT(state.ParseFlag(0, "--flag"));
  NTH_EXPECT(state.ParseFlag(0, "--flag="));
  NTH_EXPECT(state.ParseFlag(0, "--flag=xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=-xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--"));
  NTH_EXPECT(state.ParseFlag(0, "--flag=--xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--all"));
  NTH_EXPECT(state.ParseFlag(0, "--all="));
  NTH_EXPECT(state.ParseFlag(0, "--all=xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--all=-xyz"));
  NTH_EXPECT(state.ParseFlag(0, "--all=--"));
  NTH_EXPECT(state.ParseFlag(0, "--all=--xyz"));
  NTH_EXPECT(not state.ParseFlag(0, "-flag"));
  NTH_EXPECT(not state.ParseFlag(0, "f"));
  NTH_EXPECT(not state.ParseFlag(0, "--f"));
  NTH_EXPECT(not state.ParseFlag(0, "--f="));
  NTH_EXPECT(not state.ParseFlag(0, "--f=x"));
  NTH_EXPECT(state.ParseFlag(0, "-f"));
  NTH_EXPECT(state.ParseFlag(0, "-a"));
  NTH_EXPECT(not state.ParseFlag(0, "-f="));
  NTH_EXPECT(not state.ParseFlag(0, "-f=x"));
  NTH_EXPECT(not state.ParseFlag(0, "-f=-f"));
}

NTH_TEST("parse-commandline-argument") {
}

}  // namespace
}  // namespace nth::internal_commandline
