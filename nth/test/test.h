#ifndef NTH_TEST_TEST_H
#define NTH_TEST_TEST_H

#include <span>

#include "nth/base/macros.h"
#include "nth/debug/trace/trace.h"

namespace nth {

struct Test {
  Test();
  Test(Test const &)            = delete;
  Test(Test &&)                 = delete;
  Test &operator=(Test const &) = delete;
  Test &operator=(Test &&)      = delete;

  virtual void InvokeTest() const                 = 0;
  virtual std::string_view categorization() const = 0;
};

std::span<Test const *const> RegisteredTests();

}  // namespace nth

#define NTH_TEST(categorization, ...)                                          \
  NTH_INTERNAL_TEST_IMPL(NTH_CONCATENATE(NthTest, __LINE__), categorization,   \
                         __VA_ARGS__)

#define NTH_INTERNAL_TEST_IMPL(name, categorization, ...)                      \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__), NTH_INTERNAL_TEST_IMPL_,                   \
         NTH_INTERNAL_TEST_PARAMETERIZED_IMPL_, name, categorization,          \
         __VA_ARGS__)

#define NTH_INTERNAL_TEST_IMPL_(test_name, cat, ...)                           \
  struct : nth::Test {                                                         \
   private:                                                                    \
    friend ::nth::Test;                                                        \
    std::string_view categorization() const override { return cat; }           \
                                                                               \
   public:                                                                     \
    void InvokeTest() const override;                                          \
  } test_name;                                                                 \
  void std::decay_t<decltype(test_name)>::InvokeTest() const

#define NTH_INTERNAL_TEST_PARAMETERIZED_IMPL_(test_name, cat, ...)             \
  struct : nth::Test {                                                         \
   private:                                                                    \
    friend ::nth::Test;                                                        \
    std::string_view categorization() const override { return cat; }           \
                                                                               \
   public:                                                                     \
    void InvokeTest(__VA_ARGS__) const override;                               \
  } test_name;                                                                 \
  void std::decay_t<decltype(test_name)>::InvokeTest(__VA_ARGS__) const

#endif  // NTH_TEST_TEST_H
