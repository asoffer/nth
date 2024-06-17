#include "nth/test/benchmark_result.h"

#include "nth/base/indestructible.h"
#include "nth/registration/registrar.h"

namespace nth::test {
namespace {

using handler_type = void (*)(BenchmarkResult const &);
indestructible<registrar<handler_type>> registrar_;

}  // namespace

void RegisterBenchmarkResultHandler(handler_type handler) {
  registrar_->insert(handler);
}

registrar<void (*)(BenchmarkResult const &)>::range_type
RegisteredBenchmarkResultHandlers() {
  return registrar_->registry();
}

}  // namespace nth::test
