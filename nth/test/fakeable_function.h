#ifndef NTH_TEST_FAKEABLE_FUNCTION_H
#define NTH_TEST_FAKEABLE_FUNCTION_H

#include <type_traits>

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/debug/fakeable_function.h"
#include "nth/meta/type.h"
#include "nth/utility/any_invocable.h"

#if NTH_SUPPORTS_ATTRIBUTE(weak)

namespace nth::test {
namespace internal_fakeable_function {

template <typename>
struct fake;

template <typename Fn>
struct fake_resetter {
  ~fake_resetter() { f_->reset(std::move(prev_)); }

  fake_resetter(fake_resetter const &)            = delete;
  fake_resetter(fake_resetter &)                  = delete;
  fake_resetter &operator=(fake_resetter const &) = delete;
  fake_resetter &operator=(fake_resetter &)       = delete;

 private:
  template <typename>
  friend struct fake;

  fake_resetter(fake<Fn> *f, ::nth::any_invocable<Fn> prev)
      : f_(f), prev_(std::move(prev)) {}

  fake<Fn> *f_;
  ::nth::any_invocable<Fn> prev_;
};

template <typename Fn>
struct fake : private ::nth::any_invocable<Fn> {
  using ::nth::any_invocable<Fn>::operator();
  using ::nth::any_invocable<Fn>::operator bool;

  [[nodiscard]] fake_resetter<Fn> with(::nth::any_invocable<Fn> f) {
    auto *ptr = static_cast<::nth::any_invocable<Fn> *>(this);
    return fake_resetter<Fn>(this, std::exchange(*ptr, std::move(f)));
  }

 private:
  friend fake_resetter<Fn>;

  void reset(::nth::any_invocable<Fn> f) {
    *static_cast<::nth::any_invocable<Fn> *>(this) = std::move(f);
  }
};

}  // namespace internal_fakeable_function

template <auto *F>
thread_local inline internal_fakeable_function::fake<
    std::remove_pointer_t<decltype(F)>>
    fake;

}  // namespace nth::test

#define NTH_TEST_INTERNAL_NAME_BODY(type, variable) variable
#define NTH_TEST_INTERNAL_NAME_EXPAND_FIRST(x, y)                              \
  NTH_TEST_INTERNAL_NAME_BODY(x, y) NTH_TEST_INTERNAL_NAME_EXPAND_B
#define NTH_TEST_INTERNAL_NAME_EXPAND_A(x, y)                                  \
  , NTH_TEST_INTERNAL_NAME_BODY(x, y) NTH_TEST_INTERNAL_NAME_EXPAND_B
#define NTH_TEST_INTERNAL_NAME_EXPAND_B(x, y)                                  \
  , NTH_TEST_INTERNAL_NAME_BODY(x, y) NTH_TEST_INTERNAL_NAME_EXPAND_A
#define NTH_TEST_INTERNAL_NAME_EXPAND_A_END
#define NTH_TEST_INTERNAL_NAME_EXPAND_B_END
#define NTH_TEST_INTERNAL_NAME_END(...)                                        \
  NTH_TEST_INTERNAL_NAME_END_IMPL(__VA_ARGS__)
#define NTH_TEST_INTERNAL_NAME_END_IMPL(...) __VA_ARGS__##_END

#define NTH_FAKE_IMPLEMENTATION(return_type, name, argument_list)              \
  NTH_IGNORE_PARENTHESES(return_type)                                          \
  name(NTH_TEST_INTERNAL_NAMED_SIGNATURE_END(                                  \
      NTH_TEST_INTERNAL_NAMED_SIGNATURE_EXPAND_FIRST argument_list)) {         \
    if (not ::nth::test::fake<name>) { std::abort(); }                         \
    return ::nth::test::fake<name>(NTH_TEST_INTERNAL_NAME_END(                 \
        NTH_TEST_INTERNAL_NAME_EXPAND_FIRST argument_list));                   \
  }                                                                            \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                                    \
  static_assert(true)

#else  // NTH_SUPPORTS_ATTRIBUTE(weak)

#define NTH_FAKE_IMPLEMENTATION(return_type, name, ...)                        \
  static_assert(false,                                                         \
                "`NTH_FAKE_IMPLEMENTATION` requires the `weak` attribute, "    \
                "but `NTH_SUPPORTS_ATTRIBUTE(weak)` evaluates to `false`.")

#endif  // NTH_SUPPORTS_ATTRIBUTE(weak)

#endif  // NTH_TEST_FAKEABLE_FUNCTION_H
