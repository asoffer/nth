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
T &&Store(void const **destination, T &&t) {
  *destination = std::addressof(t);
  return std::forward<T>(t);
}

}  // namespace internal_fuzz

struct FuzzingContext {
  absl::BitGen &generator() const { return gen_; }

  template <typename T>
  T const &argument(size_t n) const {
    return *static_cast<T const *>(arguments_[n]);
  }
  void set_arguments(void const *const *arguments) { arguments_ = arguments; }

 private:
  // TODO: Protect with a mutex.
  mutable absl::BitGen gen_;
  void const *const *arguments_;
};

template <typename T>
struct FuzzDomain {
  using value_type = T;
};

namespace internal_fuzz {

template <typename T>
struct Just : FuzzDomain<T> {
  Just(T t) : value_(std::move(t)) {}
  T operator()(FuzzingContext const &) const { return value_; }

 private:
  T value_;
};

template <typename T, size_t N>
struct Argument : FuzzDomain<T> {
  T operator()(FuzzingContext const &context) const {
    return context.argument<T>(N);
  }

 private:
  T const *ptr_;
};

template <typename L, typename H>
struct InRange : FuzzDomain<typename L::value_type> {
  InRange(L low, H high) : low_(std::move(low)), high_(std::move(high)) {}

  auto operator()(FuzzingContext const &context) const {
    auto l = low_(context);
    auto h = high_(context);
    return absl::Uniform(absl::IntervalClosedClosed, context.generator(), l, h);
  }

 private:
  L low_;
  H high_;
};

template <typename D>
using domain_type = nth::type_t<[] {
  if constexpr (not requires { typename D::value_type; }) {
    return nth::type<Just<D>>;
  } else if constexpr (not std::derived_from<
                           D, FuzzDomain<typename D::value_type>>) {
    return nth::type<Just<D>>;
  } else {
    return nth::type<D>;
  }
}()>;

template <typename D>
decltype(auto) GetDomain(D &&d) {
  using stripped = typename std::remove_reference_t<D>;
  if constexpr (requires { typename stripped::value_type; }) {
    if constexpr (std::derived_from<
                      stripped, FuzzDomain<typename stripped::value_type>>) {
      return std::forward<D>(d);
    } else {
      return Just(std::forward<D>(d));
    }
  } else {
    return Just(std::forward<D>(d));
  }
}

template <int &..., typename... Domains>
TestArgumentGenerator<typename Domains::value_type...> FuzzyImpl(
    Domains... domains) {
  FuzzingContext context;
  void const *arguments[sizeof...(Domains)] = {};
  context.set_arguments(arguments);

  for (int i = 0; i < 3; ++i) {
    // https://github.com/llvm/llvm-project/issues/56768
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsequenced"
    void const **p = arguments;
    co_yield nth::TestArguments<typename Domains::value_type...>{
        internal_fuzz::Store(p++, domains(context))...};
#pragma clang diagnostic pop
  }
}

}  // namespace internal_fuzz

template <int &..., typename... Ts>
auto Fuzzy(Ts... ts) {
  return internal_fuzz::FuzzyImpl(internal_fuzz::GetDomain(std::move(ts))...);
}

template <typename T>
struct Any : FuzzDomain<T> {
  static_assert(std::is_arithmetic_v<T>);
  T operator()(FuzzingContext const &context) const {
    return absl::Uniform<T>(absl::IntervalClosedClosed, context.generator(),
                            std::numeric_limits<T>::lowest(),
                            std::numeric_limits<T>::max());
  }
};

template <typename L, typename R>
auto InRange(L l, R r) {
  return internal_fuzz::InRange(internal_fuzz::GetDomain(std::move(l)),
                                internal_fuzz::GetDomain(std::move(r)));
}

template <typename T>
auto AtMost(T t) {
  return internal_fuzz::InRange(
      internal_fuzz::GetDomain(std::numeric_limits<T>::lowest()),
      internal_fuzz::GetDomain(std::move(t)));
}

template <typename T>
auto AtLeast(T t) {
  using D          = internal_fuzz::domain_type<T>;
  using value_type = typename D::value_type;
  return internal_fuzz::InRange(
      internal_fuzz::GetDomain(std::move(t)),
      internal_fuzz::GetDomain(std::numeric_limits<value_type>::max()));
}

template <typename T, size_t N>
auto Argument() {
  return internal_fuzz::Argument<T, N>();
}

}  // namespace nth::test

#endif  // NTH_TEST_FUZZ_H
