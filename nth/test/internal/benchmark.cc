#include "nth/test/internal/benchmark.h"

#include "nth/test/benchmark_result.h"

namespace nth::test::internal_benchmark {

MeasurementStrategy::~MeasurementStrategy() {
  for (const auto& [name, samples] : samples_) {
    size_t total        = 0;
    size_t square_total = 0;
    for (auto d : *samples) {
      total += d;
      square_total += d * d;
    }

    size_t size = samples->size();

    test::BenchmarkResult results{
        .name     = name,
        .samples  = size,
        .mean     = static_cast<double>(total) / size,
        .variance = (static_cast<double>(square_total) / size) -
                    (static_cast<double>(total) * total / (size * size)),
    };
    samples->clear();

    for (auto handler : test::RegisteredBenchmarkResultHandlers()) {
      handler(results);
    }
  }
}

}  // namespace nth::test::internal_benchmark
