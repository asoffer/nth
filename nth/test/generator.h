#ifndef NTH_TEST_GENERATOR_H
#define NTH_TEST_GENERATOR_H

#include <coroutine>

#include "nth/test/arguments.h"

namespace nth::test {

namespace internal_generator {

struct FinalSuspension {
  explicit FinalSuspension(std::coroutine_handle<> handle) : handle_(handle) {}
  bool await_ready() const noexcept { return false; }
  std::coroutine_handle<> await_suspend(
      std::coroutine_handle<>) const noexcept {
    return handle_;
  }
  void await_resume() const noexcept {}

 private:
  std::coroutine_handle<> handle_;
};

}  // namespace internal_generator

// `TestArgumentGenerator<Ts...>` is a coroutine type which can be `co_await`ed
// from within either `NTH_INVOKE_TEST`, or another
// `TestArgumentGenerator<Ts...>`. Within such a coroutine,
// `nth::TestArgument<Ts...>` can be `co_yield`ed. All such values will be used
// to invoke all tests matched by the `NTH_INVOKE_TEST` from which it is
// (transitively) awaited.
template <typename... Ts>
struct TestArgumentGenerator {
  struct promise_type;

  void set_parent(std::coroutine_handle<> handle) {
    handle_.promise().parent_ = handle;
  }

  std::coroutine_handle<promise_type> handle() const { return handle_; }
  void set_invocable(std::function<void(Ts...)> f) {
    handle_.promise().invocable_ = std::move(f);
  }

 private:
  explicit TestArgumentGenerator(std::coroutine_handle<promise_type> handle)
      : handle_(handle) {}

  std::coroutine_handle<promise_type> handle_;
};

// Promise-type implementation.
template <typename... Ts>
struct TestArgumentGenerator<Ts...>::promise_type {
  std::suspend_always initial_suspend() const noexcept { return {}; }
  internal_generator::FinalSuspension final_suspend() const noexcept {
    return internal_generator::FinalSuspension(parent_);
  }
  void unhandled_exception() const noexcept {}

  TestArgumentGenerator get_return_object() {
    return TestArgumentGenerator(
        std::coroutine_handle<promise_type>::from_promise(*this));
  }

  std::suspend_never yield_value(
      ::nth::internal_test::IsTestArguments auto const& arguments) {
    arguments.apply(invocable_);
    return {};
  }

  std::function<void(Ts...)> invocable_;
  std::coroutine_handle<> parent_;
};

}  // namespace nth::test

#endif  // NTH_TEST_GENERATOR_H
