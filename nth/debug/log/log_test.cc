#include "nth/debug/log/log.h"

#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"

void NoVerbosityPathSpecified(std::vector<nth::log_entry> const &log) {
  nth::log_verbosity_on("**");

#line 100
  NTH_LOG("No interpolation");
#line 200
  NTH_LOG("No interpolation with argument.") <<= {};
#line 300
  NTH_LOG("Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG("Interpolation with arguments = {}, {}.") <<= {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 4);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[0].id()].source_location().line() ==
      100);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[1].id()].source_location().line() ==
      200);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[2].id()].source_location().line() ==
      300);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[3].id()].source_location().line() ==
      400);
}

void EnabledVerbosityPath(std::vector<nth::log_entry> const &log) {
  nth::log_verbosity_on("**");

#line 100
  NTH_LOG(("verbosity/path"), "No interpolation");
#line 200
  NTH_LOG(("verbosity/path"), "No interpolation with argument.") <<= {};
#line 300
  NTH_LOG(("verbosity/path"), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG(("verbosity/path"), "Interpolation with arguments = {}, {}.") <<=
      {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 4);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[0].id()].source_location().line() ==
      100);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[1].id()].source_location().line() ==
      200);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[2].id()].source_location().line() ==
      300);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[3].id()].source_location().line() ==
      400);
}

void DisabledVerbosityPath(std::vector<nth::log_entry> const &log) {
  nth::log_verbosity_off("**");

#line 100
  NTH_LOG(("verbosity/path"), "No interpolation");
#line 200
  NTH_LOG(("verbosity/path"), "No interpolation with argument.") <<= {};
#line 300
  NTH_LOG(("verbosity/path"), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG(("verbosity/path"), "Interpolation with arguments = {}, {}.") <<=
      {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 0);
}

void ModifiedVerbosityPath(std::vector<nth::log_entry> const &log) {
  nth::log_verbosity_on("**");
#line 100
  NTH_LOG(("verbosity/path"), "No interpolation");
#line 200
  NTH_LOG(("verbosity/path"), "No interpolation with argument.") <<= {};

  nth::log_verbosity_off("**");

#line 300
  NTH_LOG(("verbosity/path"), "Interpolation with argument = {}.") <<= {3};

  nth::log_verbosity_on("**");

#line 400
  NTH_LOG(("verbosity/path"), "Interpolation with arguments = {}, {}.") <<=
      {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 3);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[0].id()].source_location().line() ==
      100);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[1].id()].source_location().line() ==
      200);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[2].id()].source_location().line() ==
      400);
}

void MixedVerbosityPath(std::vector<nth::log_entry> const &log) {
  nth::log_verbosity_off("**");
  nth::log_verbosity_on("a/*");
  nth::log_verbosity_off("*/1");

#line 100
  NTH_LOG(("a/1"), "No interpolation");
#line 200
  NTH_LOG(("a/2"), "No interpolation with argument.") <<= {};
#line 300
  NTH_LOG(("b/1"), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG(("b/2"), "Interpolation with arguments = {}, {}.") <<= {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 1);

  NTH_RAW_TEST_ASSERT(
      nth::section<"nth_log_line">[log[0].id()].source_location().line() ==
      200);
}

int main() {
  std::vector<nth::log_entry> log;
  nth::vector_log_sink sink(log);
  nth::register_log_sink(sink);

  log.clear();
  NoVerbosityPathSpecified(log);

  log.clear();
  EnabledVerbosityPath(log);

  log.clear();
  DisabledVerbosityPath(log);

  log.clear();
  ModifiedVerbosityPath(log);

  log.clear();
  MixedVerbosityPath(log);

  return 0;
}
