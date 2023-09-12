#ifndef NTH_TEST_INTERNAL_ARGUMENTS_H
#define NTH_TEST_INTERNAL_ARGUMENTS_H

namespace nth::internal_test {

template <typename T>
concept IsTestArguments = requires {
  typename T::NthInternalIsTestArguments;
};

}  // namespace nth::internal_test

#endif  // NTH_TEST_INTERNAL_ARGUMENTS_H
