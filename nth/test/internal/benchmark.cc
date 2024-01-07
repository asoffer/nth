#include "nth/test/internal/benchmark.h"

#include "nth/test/benchmark_result.h"

namespace nth::test::internal_benchmark {

void SamplerBase::Register(std::string_view name, std::vector<size_t>* sample) {
  samples_.emplace_back(name, sample);
}

SamplerBase::~SamplerBase() {
  for (const auto& [name, samples] : samples_) {
    size_t total = 0;
    for (auto d : *samples) { total += d; }
    size_t size     = samples->size();
    double mean     = static_cast<double>(total) / size;
    double variance = 0;
    for (auto d : *samples) { variance += (d - mean) * (d - mean); }
    variance /= (size - 1);
    test::BenchmarkResult results{
        .name = name, .samples = size, .mean = mean, .variance = variance};
    samples->clear();

    for (auto handler : test::RegisteredBenchmarkResultHandlers()) {
      handler(results);
    }
  }
}

}  // namespace nth::test::internal_benchmark
