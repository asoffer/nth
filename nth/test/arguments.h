#ifndef NTH_TEST_ARGUMENTS_H
#define NTH_TEST_ARGUMENTS_H

#include <memory>
#include <type_traits>
#include <utility>

#include "nth/test/internal/arguments.h"

namespace nth {

// Represents multiple arguments to be `co_yield` from an `NTH_INVOKE_TEST`
// which will be used to instantiate all matching tests. If a test accepts a
// single argument, there is no need to wrap that argument in `TestArguments`.
// Test arguments only need to be wrapped when there are multiple arguments to
// indicate that they should be expanded into the test parameters.
//
// Test arguments are not copyable, movable, or mutable. The only supported use
// of them is to construct them as the operand to a `co_yield` expression
// directly.
template <typename... Ts>
struct TestArguments {
  using NthInternalIsTestArguments = void;

  TestArguments(TestArguments const &)            = delete;
  TestArguments(TestArguments &&)                 = delete;
  TestArguments &operator=(TestArguments const &) = delete;
  TestArguments &operator=(TestArguments &&)      = delete;

  explicit TestArguments(Ts const &...arguments)
      : argument_pointers_{std::addressof(arguments)...} {}

  template <typename F>
  auto apply(F &&f) const {
    return [&]<size_t... Ns>(std::index_sequence<Ns...>) {
      std::forward<F>(f)(
          std::forward<Ts>(*static_cast<std::remove_reference_t<Ts> *>(
              const_cast<void *>(argument_pointers_[Ns])))...);
    }
    (std::make_index_sequence<sizeof...(Ts)>{});
  }

 private:
  void const *argument_pointers_[sizeof...(Ts)];
};

template <typename... Ts>
TestArguments(Ts &&...) -> TestArguments<Ts...>;

}  // namespace nth

#endif  // NTH_TEST_ARGUMENTS_H
