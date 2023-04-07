#include "nth/utility/required.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

struct S {
  int field = required;
};

TEST(Required, AllowsFieldToBePopulated) {
  EXPECT_EQ(S{3}.field, 3);
  EXPECT_EQ(S{.field = 3}.field, 3);
  // TODO: We need a non-compile test to verify that this does what we expect it
  // to do.
}

}  // namespace
}  // namespace nth
