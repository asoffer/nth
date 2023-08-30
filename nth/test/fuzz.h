#ifndef NTH_TEST_FUZZ_H
#define NTH_TEST_FUZZ_H

#include <concepts>
#include <utility>

#include "absl/random/random.h"
#include "nth/test/generator.h"
#include "nth/test/internal/invocation.h"

namespace nth::test {
namespace internal_fuzz {

template <typename T>
concept FuzzDomain = requires {
  typename T::value_type;
};

}  // namespace internal_fuzz

template <typename T>
struct Any {
  using value_type = T;

  value_type operator()() const {
    return absl::Uniform(absl::IntervalClosedOpen, *generator_, 0, 10);
  }

  explicit Any() = default;

  void set_generator(absl::BitGen &generator) { generator_ = &generator; }

 private:
  absl::BitGen *generator_;
};

template <int &..., internal_fuzz::FuzzDomain... Domains>
TestArgumentGenerator<typename Domains::value_type...> Fuzzy(
    Domains &&...domains) {
  absl::BitGen generator;
  (domains.set_generator(generator), ...);

  for (int i = 0; i < 3; ++i) { co_yield nth::TestArguments{domains()...}; }
}

}  // namespace nth::test

#endif  // NTH_TEST_FUZZ_H
