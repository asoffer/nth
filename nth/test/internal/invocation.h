#ifndef NTH_TEST_INTERNAL_INVOCATION_H
#define NTH_TEST_INTERNAL_INVOCATION_H

#include <coroutine>

namespace nth::internal_test {

template <typename T, typename Generator>
struct Awaitable {
  Awaitable(Generator &g) : generator_(g) {}

  constexpr bool await_ready() const { return false; }

  void await_resume() const {}

  std::coroutine_handle<typename Generator::promise_type> await_suspend(
      std::coroutine_handle<> h) {
    generator_.set_invocable(
        []<typename... Arguments>(Arguments &&...arguments) {
          T::InvokeTest(std::forward<Arguments>(arguments)...);
        });
    generator_.set_parent(h);
    return generator_.handle();
  }

 private:
  Generator &generator_;
};

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

    template <typename Generator>
    auto await_transform(Generator &&generator) {
      return Awaitable<T, std::remove_reference_t<Generator>>(generator);
    }

    TestInvocation get_return_object() { return {}; }
  };
};

}  // namespace nth::internal_test

#endif  // NTH_TEST_INTERNAL_INVOCATION_H
