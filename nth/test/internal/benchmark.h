#ifndef NTH_TEST_INTERNAL_BENCHMARK_H
#define NTH_TEST_INTERNAL_BENCHMARK_H

#include <chrono>
#include <string_view>
#include <utility>
#include <vector>

#include "nth/meta/compile_time_string.h"

namespace nth::test::internal_benchmark {

struct MeasurementStrategy {
  bool done() const { return count_ == 1000010; }
  bool should_sample() const { return count_ >= 10; }
  void next() { ++count_; }

 private:
  size_t count_ = 0;
};

struct SamplerBase {
  void Register(std::string_view name, std::vector<size_t>* sample);
  ~SamplerBase();

 private:
  std::vector<std::pair<std::string_view, std::vector<size_t>*>> samples_;
};

template <typename S>
struct Sampler : SamplerBase {
  S& strategy() { return strategy_; }
  S const& strategy() const { return strategy_; }

 private:
  S strategy_;
};

struct TimeBlockStateBase {
  explicit TimeBlockStateBase(Sampler<MeasurementStrategy>& s) : sampler_(s) {}

  void start_sample(std::chrono::high_resolution_clock::time_point start) {
    start_ = start;
  }
  bool ready() { return ready_ = not ready_; }
  void add_sample(std::chrono::high_resolution_clock::time_point end) {
    if (sampler_.strategy().should_sample()) {
      sample_.push_back((end - start_).count());
    }
  }

 protected:
  Sampler<MeasurementStrategy>& sampler_;
  std::chrono::high_resolution_clock::time_point start_{};
  std::vector<size_t> sample_;
  bool ready_ = false;
};

template <compile_time_string Name>
struct TimeBlockState : TimeBlockStateBase {
  explicit TimeBlockState(Sampler<MeasurementStrategy>& s)
      : TimeBlockStateBase(s) {
    sampler_.Register(Name, &sample_);
  }
};

}  // namespace nth::test::internal_benchmark

#define NTH_TEST_INTERNAL_TIME(loc)                                            \
  if constexpr ((NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block,  \
                 false)) {                                                     \
  } else                                                                       \
    for (static ::nth::test::internal_benchmark::TimeBlockState<loc>           \
             NthInternalTimeBlockState(                                        \
                 NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block); \
         NthInternalTimeBlockState.ready()                                     \
             ? (NthInternalTimeBlockState.start_sample(                        \
                    ::std::chrono::high_resolution_clock::now()),              \
                true)                                                          \
             : (NthInternalTimeBlockState.add_sample(                          \
                    ::std::chrono::high_resolution_clock::now()),              \
                false);)

#define NTH_TEST_INTERNAL_MEASURE(...)                                         \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_TEST_INTERNAL_MEASURE_IMPL_WITH_DEFAULT(),                        \
         NTH_TEST_INTERNAL_MEASURE_IMPL(__VA_ARGS__))

#define NTH_TEST_INTERNAL_MEASURE_IMPL_WITH_DEFAULT()                          \
  NTH_TEST_INTERNAL_MEASURE_IMPL(                                              \
      ::nth::test::internal_benchmark::MeasurementStrategy)

#define NTH_TEST_INTERNAL_MEASURE_IMPL(...)                                    \
  for (::nth::test::internal_benchmark::Sampler<__VA_ARGS__>                   \
           NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block;        \
       not NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block         \
               .strategy()                                                     \
               .done();                                                        \
       NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block.strategy()  \
           .next())

#endif  // NTH_TEST_INTERNAL_BENCHMARK_H
