#include "nth/debug/log/log.h"

#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"

void AlwaysLog(std::vector<nth::log_entry> const &log) {
#line 100
  NTH_LOG((v.always), "No interpolation");
#line 200
  NTH_LOG((v.always), "No interpolation with argument.") <<= {};
#line 300
  NTH_LOG((v.always), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG((v.always), "Interpolation with arguments = {}, {}.") <<=
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

// TODO: verbosity.
void NeverLog(std::vector<nth::log_entry> const &) {
#if 0
#line 100
  NTH_LOG((v.never), "No interpolation");
#line 200
  NTH_LOG((v.never), "No interpolation with argument") <<= {};
#line 300
  NTH_LOG((v.never), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG((v.never), "Interpolation with arguments = {}, {}.") <<= {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 0);
#endif
}

int main() {
  std::vector<nth::log_entry> log;
  nth::VectorLogSink sink(log);
  nth::register_log_sink(sink);

  log.clear();
  AlwaysLog(log);

  log.clear();
  NeverLog(log);
  return 0;
}

