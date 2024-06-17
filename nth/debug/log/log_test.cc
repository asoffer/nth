#include "nth/debug/log/log.h"

#include "nth/debug/log/vector_log_sink.h"
#include "nth/test/raw/test.h"

int AlwaysLog() {
  std::vector<nth::log_entry> log;
  nth::VectorLogSink sink(log);
  nth::register_log_sink(sink);

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

  NTH_RAW_TEST_ASSERT(log[0].id().line().metadata().source_location().line() ==
                      100);

  NTH_RAW_TEST_ASSERT(log[1].id().line().metadata().source_location().line() ==
                      200);

  NTH_RAW_TEST_ASSERT(log[2].id().line().metadata().source_location().line() ==
                      300);

  NTH_RAW_TEST_ASSERT(log[3].id().line().metadata().source_location().line() ==
                      400);

  return 0;
}

int NeverLog() {
  std::vector<nth::log_entry> log;
  nth::VectorLogSink sink(log);
  nth::register_log_sink(sink);

#line 100
  NTH_LOG((v.never), "No interpolation");
#line 200
  NTH_LOG((v.never), "No interpolation with argument") <<= {};
#line 300
  NTH_LOG((v.never), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG((v.never), "Interpolation with arguments = {}, {}.") <<= {4, "hello"};

  NTH_RAW_TEST_ASSERT(log.size() == 0);
  return 0;
}

int main() {
  NTH_RAW_TEST_ASSERT(AlwaysLog() == 0);
  NTH_RAW_TEST_ASSERT(NeverLog() == 0);
  return 0;
}

