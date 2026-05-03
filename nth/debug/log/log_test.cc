#include "nth/debug/log/log.h"

#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"

void NoVerbosityPathSpecified(std::vector<nth::log_entry> const& log) {
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

void EnabledVerbosityPath(std::vector<nth::log_entry> const& log) {
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

void DisabledVerbosityPath(std::vector<nth::log_entry> const& log) {
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

void ModifiedVerbosityPath(std::vector<nth::log_entry> const& log) {
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

void MixedVerbosityPath(std::vector<nth::log_entry> const& log) {
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

void ConditionalLogging(std::vector<nth::log_entry> const& log) {
  struct condition {
    using nth_log_config_type = std::string;

    static std::shared_ptr<std::string> parse(std::string_view s) {
      return std::make_shared<std::string>(s);
    }

    bool operator()(std::string_view s) const { return s == match; }

    std::string match;
  } cond;

  nth::log_verbosity_on("**", "key");

  int eval_count = 0;

  cond.match = "";
  NTH_LOG(("a/1", cond), "{}") <<= [&] {
    ++eval_count;
    return "Skipped";
  }();
  NTH_RAW_TEST_ASSERT(log.size() == 0);
  NTH_RAW_TEST_ASSERT(eval_count == 0);

  cond.match = "key";
  NTH_LOG(("a/1", cond), "{}") <<= [&] {
    ++eval_count;
    return "Logged";
  }();
  NTH_RAW_TEST_ASSERT(log.size() == 1);
  NTH_RAW_TEST_ASSERT(eval_count == 1);
}

void ConditionalLoggingWithNontrivialParse(
    std::vector<nth::log_entry> const& log) {
  enum class Color { Red = 1, Green = 2, Blue = 4 };
  struct condition {
    using nth_log_config_type = Color;

    static std::shared_ptr<Color> parse(std::string_view s) {
      if (s == "red") {
        return std::make_shared<Color>(Color::Red);
      } else if (s == "green") {
        return std::make_shared<Color>(Color::Green);
      } else if (s == "blue") {
        return std::make_shared<Color>(Color::Blue);
      } else if (s == "all") {
        return std::make_shared<Color>(static_cast<Color>(7));
      } else {
        return nullptr;
      }
    }

    bool operator()(Color c) const {
      return (static_cast<int>(c) & static_cast<int>(color)) ==
             static_cast<int>(color);
    }

    Color color;
  } cond;

  nth::log_verbosity_on("**", "red");
  cond.color = Color::Red;
  NTH_LOG(("", cond), "Red!");
  cond.color = Color::Green;
  NTH_LOG(("", cond), "Green!");
  cond.color = Color::Blue;
  NTH_LOG(("", cond), "Blue!");
  NTH_RAW_TEST_ASSERT(log.size() == 1);

  nth::log_verbosity_on("**", "green");
  cond.color = Color::Red;
  NTH_LOG(("", cond), "Red!");
  cond.color = Color::Green;
  NTH_LOG(("", cond), "Green!");
  cond.color = Color::Blue;
  NTH_LOG(("", cond), "Blue!");
  NTH_RAW_TEST_ASSERT(log.size() == 2);

  nth::log_verbosity_on("**", "all");
  cond.color = Color::Red;
  NTH_LOG(("", cond), "Red!");
  cond.color = Color::Green;
  NTH_LOG(("", cond), "Green!");
  cond.color = Color::Blue;
  NTH_LOG(("", cond), "Blue!");
  NTH_RAW_TEST_ASSERT(log.size() == 5);
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

  log.clear();
  ConditionalLogging(log);

  log.clear();
  ConditionalLoggingWithNontrivialParse(log);

  return 0;
}
