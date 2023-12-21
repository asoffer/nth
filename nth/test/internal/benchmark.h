#ifndef NTH_TEST_INTERNAL_BENCHMARK_H
#define NTH_TEST_INTERNAL_BENCHMARK_H

#include <chrono>
#include <string_view>
#include <utility>
#include <vector>

#include "nth/meta/compile_time_string.h"

namespace nth::test::internal_benchmark {

struct MeasurementStrategy {
  bool done() const { return count_ == 10010; }
  bool should_sample() const { return count_ >= 10; }
  void next() { ++count_; }

  ~MeasurementStrategy();

  void Register(std::string_view name, std::vector<size_t>* sample) {
    samples_.emplace_back(name, sample);
  }

 private:
  std::vector<std::pair<std::string_view, std::vector<size_t>*>> samples_;
  size_t count_ = 0;
};

struct TimeBlockStateBase {
  explicit TimeBlockStateBase(MeasurementStrategy& s) : strategy_(s) {}

  void start_sample(std::chrono::high_resolution_clock::time_point start) {
    start_ = start;
  }
  bool ready() { return ready_ = not ready_; }
  void add_sample(std::chrono::high_resolution_clock::time_point end) {
    if (strategy_.should_sample()) {
      sample_.push_back((end - start_).count());
    }
  }

 protected:
  MeasurementStrategy& strategy_;
  std::chrono::high_resolution_clock::time_point start_{};
  std::vector<size_t> sample_;
  bool ready_ = false;
};

template <CompileTimeString Name>
struct TimeBlockState : TimeBlockStateBase {
  explicit TimeBlockState(MeasurementStrategy& s) : TimeBlockStateBase(s) {
    strategy_.Register(Name, &sample_);
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

#define NTH_TEST_INTERNAL_MEASURE()                                            \
  for (::nth::test::internal_benchmark::MeasurementStrategy                    \
           NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block;        \
       not NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block.done(); \
       NthInternal_NTH_TIME_MustBeDeclaredInside_NTH_MEASURE_Block.next())

#endif  // NTH_TEST_INTERNAL_BENCHMARK_H
