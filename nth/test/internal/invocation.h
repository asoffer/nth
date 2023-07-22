#ifndef NTH_TEST_INTERNAL_INVOCATION_H
#define NTH_TEST_INTERNAL_INVOCATION_H

#include <coroutine>

namespace nth::internal_test {

template <typename T>
struct TestInvocation {
  struct promise_type {
    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }
    void unhandled_exception() const noexcept {}

    template <typename U>
    requires(not ::nth::internal_test::IsTestArguments<U>) std::suspend_never
        yield_value(U &&argument) {
      T::InvokeTest(std::forward<U>(argument));
      return {};
    }

    std::suspend_never yield_value(
        ::nth::internal_test::IsTestArguments auto &&arguments) {
      arguments.apply([]<typename... Ts>(Ts &&...args) {
        T::InvokeTest(std::forward<Ts>(args)...);
      });
      return {};
    }

    TestInvocation get_return_object() { return {}; }
  };
};

}  // namespace nth::internal_test

#endif  // NTH_TEST_INTERNAL_INVOCATION_H
