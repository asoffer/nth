#include "nth/interval/interval.h"

#include <string>

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(Interval, Construction) {
  Interval n(3, 5);
  EXPECT_EQ(n.start(), 3);
  EXPECT_EQ(n.end(), 5);

  auto [s, e] = n;
  EXPECT_EQ(s, 3);
  EXPECT_EQ(e, 5);
}

TEST(Interval, Length) {
  Interval n(3, 5);
  EXPECT_EQ(n.length(), 2);
}

}  // namespace
}  // namespace nth
