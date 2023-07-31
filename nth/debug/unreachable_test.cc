#include "nth/debug/log/vector_log_sink.h"

static int unreachable_count = 0;
#define NTH_DEBUG_INTERNAL_TEST_UNREACHABLE (+[] { ++unreachable_count; })

#include "nth/debug/unreachable.h"

template <typename Iter>
size_t ComponentCount(Iter b, Iter e) {
  size_t count = 0;
  while (b != e) {
    ++b;
    ++count;
  }
  return count;
}

void no_logging() { NTH_UNREACHABLE(); }
void logging_without_verbosity() { NTH_UNREACHABLE("{}") <<= {3}; }
void log_with_verbosity_off() { NTH_UNREACHABLE((v.never), "{}") <<= {3}; }
void log_with_verbosity_on() { NTH_UNREACHABLE((v.always), ""); }

int main() {
  std::vector<nth::LogEntry> log;
  nth::VectorLogSink sink(log);
  nth::RegisterLogSink(sink);

  no_logging();
  if (unreachable_count != 1) { return 1; }
  if (log.size() != 0) { return 1; }

  logging_without_verbosity();
  if (unreachable_count != 2) { return 1; }
  if (log.size() != 1) { return 1; }
  if (ComponentCount(log[0].component_begin(), log[0].component_end()) != 1) {
    return 1;
  }

  log_with_verbosity_off();
  if (unreachable_count != 3) { return 1; }
  if (log.size() != 1) { return 1; }

  log_with_verbosity_on();
  if (unreachable_count != 4) { return 1; }
  if (log.size() != 2) { return 1; }
  if (ComponentCount(log[1].component_begin(), log[1].component_end()) != 0) {
    return 1;
  }

  return 0;
}
