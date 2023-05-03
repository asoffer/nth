#include "nth/utility/projected_span.h"

#include <array>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace nth {
namespace {

using ::testing::ElementsAre;
using ::testing::Pointee;

TEST(ProjectedSpan, ContainerConstruction) {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<[](int const& n) { return &n; }> span(a);
  EXPECT_EQ(span.size(), 5);
  EXPECT_THAT(span.front(), Pointee(1));
  EXPECT_THAT(span.back(), Pointee(5));
  EXPECT_THAT(span, ElementsAre(Pointee(1), Pointee(2), Pointee(3), Pointee(4),
                                Pointee(5)));

  std::vector<int> v{5, 4, 3, 2, 1};
  span = v;
  EXPECT_EQ(span.size(), 5);
  EXPECT_THAT(span.front(), Pointee(5));
  EXPECT_THAT(span.back(), Pointee(1));
  EXPECT_THAT(span, ElementsAre(Pointee(5), Pointee(4), Pointee(3), Pointee(2),
                                Pointee(1)));
}

TEST(ProjectedSpan, IteratorPairConstruction) {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<[](int const& n) { return &n; }> span(a.begin(), a.end());
  EXPECT_EQ(span.size(), 5);
  EXPECT_THAT(span.front(), Pointee(1));
  EXPECT_THAT(span.back(), Pointee(5));
  EXPECT_THAT(span, ElementsAre(Pointee(1), Pointee(2), Pointee(3), Pointee(4),
                                Pointee(5)));
}

TEST(ProjectedSpan, RemoveEnds) {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<[](int const& n) { return &n; }> span(a);
  span.remove_prefix(1);
  EXPECT_EQ(span.size(), 4);
  EXPECT_THAT(span.front(), Pointee(2));
  EXPECT_THAT(span.back(), Pointee(5));
  EXPECT_THAT(span,
              ElementsAre(Pointee(2), Pointee(3), Pointee(4), Pointee(5)));
  span.remove_suffix(1);
  EXPECT_EQ(span.size(), 3);
  EXPECT_THAT(span.front(), Pointee(2));
  EXPECT_THAT(span.back(), Pointee(4));
  EXPECT_THAT(span, ElementsAre(Pointee(2), Pointee(3), Pointee(4)));
}

TEST(ProjectedSpan, Empty) {
  ProjectedSpan<[](int const& n) { return &n; }> span;
  EXPECT_TRUE(span.empty());
  std::array<int, 1> a{1};
  span = a;
  EXPECT_FALSE(span.empty());
  span.remove_prefix(1);
  EXPECT_TRUE(span.empty());
}

TEST(ProjectedSpan, Data) {
  std::array<int, 1> a{1};
  ProjectedSpan<[](int const& n) { return &n; }> span(a);
  EXPECT_EQ(span.data(), a.data());
}

TEST(ProjectedSpan, CArray) {
  int a[5] = {1, 2, 3, 4, 5};
  ProjectedSpan<[](int const& n) { return &n; }> span(a);
  EXPECT_EQ(span.size(), 5);
}

TEST(ProjectedSpan, DecltypeAuto) {
  std::pair<int, int> a[3] = {{1, 2}, {3, 4}, {5, 6}};
  ProjectedSpan<[](std::pair<int, int> const& p) -> int const& {
    return p.first;
  }>
      span(a);
  EXPECT_EQ(&span.front(), &a[0].first);
  EXPECT_EQ(&span.back(), &a[2].first);
  EXPECT_EQ(&span[1], &a[1].first);
}

}  // namespace
}  // namespace nth
