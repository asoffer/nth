#include "nth/strings/edit_distance.h"

#include "gtest/gtest.h"

namespace nth {

TEST(LevenshteinDistance, EmptyStrings) {
  EXPECT_EQ(LevenshteinDistance("", ""), 0);
  EXPECT_EQ(LevenshteinDistance("abc", ""), 3);
  EXPECT_EQ(LevenshteinDistance("", "xyz"), 3);
}

TEST(LevenshteinDistance, Transposition) {
  EXPECT_EQ(LevenshteinDistance("ab", "ba"), 2);
  EXPECT_EQ(LevenshteinDistance("abcdef", "bacdef"), 2);
}

TEST(LevenshteinDistance, DifferentLengths) {
  EXPECT_EQ(LevenshteinDistance("dog", "fido"), 3);
  EXPECT_EQ(LevenshteinDistance("fido", "dog"), 3);
  EXPECT_EQ(LevenshteinDistance("dog", "doggo"), 2);
  EXPECT_EQ(LevenshteinDistance("doggo", "dog"), 2);
}

}  // namespace nth
