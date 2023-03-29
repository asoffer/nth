#include "nth/utility/lazy.h"

#include <string>

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(Lazy, DoesntEvaluateIfNotConverted) {
  int n = 0;
  static_cast<void>(lazy([&n] {
    ++n;
    return true;
  }));
  EXPECT_EQ(n, 0);
}

TEST(Lazy, InvokesIfConverted) {
  int n = 0;
  EXPECT_TRUE(static_cast<bool>(lazy([&n] {
    ++n;
    return true;
  })));
  EXPECT_EQ(n, 1);
}

}  // namespace
}  // namespace nth
