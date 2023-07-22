#ifndef NTH_TEST_INTERNAL_ARGUMENTS_H
#define NTH_TEST_INTERNAL_ARGUMENTS_H

#include <memory>
#include <type_traits>
#include <utility>

namespace nth::internal_test {

struct TestArgumentBase {};

template <typename T>
concept IsTestArguments = std::derived_from<T, TestArgumentBase>;

}  // namespace nth::internal_test

#endif  // NTH_TEST_INTERNAL_ARGUMENTS_H
