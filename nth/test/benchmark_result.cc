#include "nth/test/benchmark_result.h"

namespace nth::test {
namespace {

using handler_type = void (*)(BenchmarkResult const &);
using Registrar = internal_base::Registrar<struct Key, handler_type>;

}  // namespace


void RegisterBenchmarkResultHandler(handler_type handler) {
  Registrar::Register(handler);
}

internal_base::RegistrarImpl<void (*)(BenchmarkResult const &)>::Range
RegisteredBenchmarkResultHandlers() {
  return Registrar::Registry();
}

}  // namespace nth::test
