#include "nth/debug/log/log.h"

#include "nth/debug/log/vector_log_sink.h"

#define NTH_INTERNAL_TEST_ASSERT(...)                                          \
  if (not(__VA_ARGS__)) { return 1; }

template <typename Iter>
size_t ComponentCount(Iter b, Iter e) {
  size_t count = 0;
  while (b != e) {
    ++b;
    ++count;
  }
  return count;
}

int AlwaysLog() {
  std::vector<nth::log_entry> log;
  nth::VectorLogSink sink(log);
  nth::register_log_sink(sink);

#line 100
  NTH_LOG((v.always), "No interpolation");
#line 200
  NTH_LOG((v.always), "No interpolation with arguments.") <<= {};
#line 300
  NTH_LOG((v.always), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG((v.always), "Interpolation with arguments = {}, {}.") <<=
      {4, "hello"};

  NTH_INTERNAL_TEST_ASSERT(log.size() == 4);

  NTH_INTERNAL_TEST_ASSERT(
      log[0].id().line().metadata().source_location().line() == 100);
  NTH_INTERNAL_TEST_ASSERT(
      ComponentCount(log[0].component_begin(), log[0].component_end()) == 0);

  NTH_INTERNAL_TEST_ASSERT(
      log[1].id().line().metadata().source_location().line() == 200);
  NTH_INTERNAL_TEST_ASSERT(
      ComponentCount(log[1].component_begin(), log[1].component_end()) == 0);

  NTH_INTERNAL_TEST_ASSERT(
      log[2].id().line().metadata().source_location().line() == 300);
  NTH_INTERNAL_TEST_ASSERT(
      ComponentCount(log[2].component_begin(), log[2].component_end()) == 1);
  NTH_INTERNAL_TEST_ASSERT(*log[2].component_begin() == "3");

  NTH_INTERNAL_TEST_ASSERT(
      log[3].id().line().metadata().source_location().line() == 400);
  NTH_INTERNAL_TEST_ASSERT(
      ComponentCount(log[3].component_begin(), log[3].component_end()) == 2);
  auto iter = log[3].component_begin();
  NTH_INTERNAL_TEST_ASSERT(*iter == "4");
  ++iter;
  NTH_INTERNAL_TEST_ASSERT(*iter == "hello");

  return 0;
}

int NeverLog() {
  std::vector<nth::log_entry> log;
  nth::VectorLogSink sink(log);
  nth::register_log_sink(sink);

#line 100
  NTH_LOG((v.never), "No interpolation");
#line 200
  NTH_LOG((v.never), "No interpolation with arguments.") <<= {};
#line 300
  NTH_LOG((v.never), "Interpolation with argument = {}.") <<= {3};
#line 400
  NTH_LOG((v.never), "Interpolation with arguments = {}, {}.") <<=
      {4, "hello"};

  NTH_INTERNAL_TEST_ASSERT(log.size() == 0);
  return 0;
}

int main() {
  NTH_INTERNAL_TEST_ASSERT(AlwaysLog() == 0);
  NTH_INTERNAL_TEST_ASSERT(NeverLog() == 0);
  return 0;
}
