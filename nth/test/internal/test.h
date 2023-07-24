#ifndef NTH_TEST_INTERNAL_TEST_H
#define NTH_TEST_INTERNAL_TEST_H

#include <functional>
#include <type_traits>

#include "nth/meta/stateful.h"
#include "nth/strings/glob.h"
#include "nth/test/internal/arguments.h"
#include "nth/test/internal/invocation.h"
#include "nth/utility/registration.h"

namespace nth {

// The stateful compile-time sequence of types which will be appended to once
// for each parameterized test encountered. Despite not being in an internal
// namespace, this object is for internal use only and must not be used directly
// outside of this library..
NTH_DEFINE_MUTABLE_COMPILE_TIME_SEQUENCE(NthInternalParameterizedTestSequence);

// The stateful compile-time sequence of types which will be appended to once
// for each parameterized test invocation encountered. Despite not being in an
// internal namespace, this object is for internal use only and must not be used
// directly outside of this library.
NTH_DEFINE_MUTABLE_COMPILE_TIME_SEQUENCE(
    NthInternalParameterizedTestInvocationSequence);

// Registers `f` as a test to be invoked.
void RegisterTestInvocation(std::function<void()> f);

namespace internal_test {

// Accepts an invocation type as a template argument and a sequence of all test
// types seen so far during compilation. For each test, if the test's category
// matches the invocation's glob, the invocation is instantiated for the test
// and registered.
template <typename TestInvocation>
void RegisterTestsMatching(nth::Sequence auto seq) {
  seq.each([&](auto t) {
    using type = nth::type_t<t>;
    if constexpr (nth::GlobMatches(TestInvocation::categorization(),
                                   type::categorization())) {
      RegisterTestInvocation(
          [] { TestInvocation::template Invocation<type>(); });
    }
  });
}

// Accepts a test type as a template argument and a sequence of all test
// invocations seen so far during compilation. For each invocation, if the
// test's category matches the invocation's glob, the invocation is instantiated
// for the test and registered.
template <typename Test>
void RegisterInvocationsMatching(nth::Sequence auto seq) {
  seq.each([&](auto t) {
    using type = nth::type_t<t>;
    if constexpr (nth::GlobMatches(type::categorization(),
                                   Test::categorization())) {
      RegisterTestInvocation([] { type::template Invocation<Test>(); });
    }
  });
}

}  // namespace internal_test

#define NTH_INTERNAL_TEST_IMPL(name, initializer, categorization, ...)         \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__), NTH_INTERNAL_TEST_IMPL_,                   \
         NTH_INTERNAL_TEST_PARAMETERIZED_IMPL_, name, initializer,             \
         categorization, __VA_ARGS__)

#define NTH_INTERNAL_TEST_IMPL_(test_name, ignored_initializer, cat, ...)      \
  struct test_name {                                                           \
    static std::string_view categorization() { return cat; }                   \
    static void InvokeTest();                                                  \
                                                                               \
   private:                                                                    \
    static nth::RegistrationToken const registration_token;                    \
    [[maybe_unused]] static constexpr OdrUse<&registration_token>              \
        RegisterTokenUse;                                                      \
  };                                                                           \
  inline nth::RegistrationToken const test_name::registration_token = [] {     \
    RegisterTestInvocation(&test_name::InvokeTest);                            \
  };                                                                           \
  void test_name::InvokeTest()

#define NTH_INTERNAL_TEST_PARAMETERIZED_IMPL_(test_name, initializer, cat,     \
                                              ...)                             \
  struct test_name {                                                           \
    static constexpr std::string_view categorization() { return cat; }         \
    static void InvokeTest(__VA_ARGS__);                                       \
                                                                               \
   private:                                                                    \
    static nth::RegistrationToken const registration_token;                    \
    [[maybe_unused]] static constexpr OdrUse<&registration_token>              \
        RegisterTokenUse;                                                      \
  };                                                                           \
  inline nth::RegistrationToken const test_name::registration_token = [] {     \
    ::nth::internal_test::RegisterInvocationsMatching<test_name>(              \
        *::nth::NthInternalParameterizedTestInvocationSequence);               \
    ::nth::NthInternalParameterizedTestSequence                                \
        .append<nth::type<test_name>>();                                       \
  };                                                                           \
  void test_name::InvokeTest(__VA_ARGS__)

#define NTH_INTERNAL_INVOKE_TEST_IMPL(invocation_name, initializer, cat)       \
  struct invocation_name {                                                     \
    static constexpr std::string_view categorization() { return cat; }         \
                                                                               \
    template <typename T>                                                      \
    static ::nth::internal_test::TestInvocation<T> Invocation();               \
                                                                               \
   private:                                                                    \
    static nth::RegistrationToken const registration_token;                    \
    [[maybe_unused]] static constexpr OdrUse<&registration_token>              \
        RegisterTokenUse;                                                      \
  };                                                                           \
  inline nth::RegistrationToken const invocation_name::registration_token =    \
      [] {                                                                     \
        ::nth::NthInternalParameterizedTestInvocationSequence                  \
            .append<nth::type<invocation_name>.decayed()>();                   \
        ::nth::internal_test::RegisterTestsMatching<invocation_name>(          \
            *::nth::NthInternalParameterizedTestSequence);                     \
      };                                                                       \
  template <typename T>                                                        \
  ::nth::internal_test::TestInvocation<T> invocation_name::Invocation()

}  // namespace nth

#endif  // NTH_TEST_INTERNAL_TEST_H
