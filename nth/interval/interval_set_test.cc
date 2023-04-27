#include "nth/interval/interval_set.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace nth {
namespace {

using ::testing::ElementsAre;

TEST(IntervalSet, DefaultConstruction) {
  IntervalSet<int> set;
  EXPECT_TRUE(set.empty());
}

TEST(IntervalSet, ConstructionFromInterval) {
  IntervalSet set(Interval(3, 5));
   EXPECT_FALSE(set.empty());
   EXPECT_EQ(set.length(), 2);
   EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 5)));
}

TEST(IntervalSet, Containment) {
  IntervalSet set(Interval(3, 5));
   EXPECT_FALSE(set.contains(2));
   EXPECT_TRUE(set.contains(3));
   EXPECT_TRUE(set.contains(4));
   EXPECT_FALSE(set.contains(5));

   EXPECT_TRUE(set.covers(Interval(3, 4)));
   EXPECT_TRUE(set.covers(Interval(4, 5)));
   EXPECT_TRUE(set.covers(Interval(3, 5)));
   EXPECT_FALSE(set.covers(Interval(2, 3)));
   EXPECT_FALSE(set.covers(Interval(2, 5)));
   EXPECT_FALSE(set.covers(Interval(4, 6)));
   EXPECT_FALSE(set.covers(Interval(5, 6)));
}

TEST(IntervalSet, Insertion) {
  IntervalSet<int> set;
  set.insert(Interval(3, 5));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 5)));

  set.insert(Interval(3, 5));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 5)));

  set.insert(Interval(3, 6));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 6)));

  set.insert(Interval(3, 5));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 6)));

  set.insert(Interval(6, 10));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(3, 10)));

  set.insert(Interval(0, 3));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(0, 10)));

  set.insert(Interval(40, 50));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(0, 10), Interval(40, 50)));

  set.insert(Interval(20, 30));
  EXPECT_THAT(set.intervals(),
              ElementsAre(Interval(0, 10), Interval(20, 30), Interval(40, 50)));

  set.insert(Interval(29, 41));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(0, 10), Interval(20, 50)));

  set.insert(Interval(60, 70));
  set.insert(Interval(50, 60));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(0, 10), Interval(20, 70)));

  set.insert(Interval(5, 25));
  EXPECT_THAT(set.intervals(), ElementsAre(Interval(0, 70)));
}

TEST(IntervalSet, Length) {
  IntervalSet<int> set;
  set.insert(Interval(3, 5));
  set.insert(Interval(10, 15));
  set.insert(Interval(5, 11));
  EXPECT_EQ(set.length(), 12);
}

}  // namespace
}  // namespace nth
