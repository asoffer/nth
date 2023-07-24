#include "nth/utility/projected_span.h"

#include <array>
#include <vector>

#include "nth/test/test.h"

namespace nth {
namespace {

template <typename T>
concept HasConstIterator = requires {
  typename T::const_iterator;
};
template <typename T>
concept HasIterator = requires {
  typename T::iterator;
};

static_assert(
    HasConstIterator<ProjectedSpan<int, [](int const& n) { return &n; }>>);
static_assert(
    not HasIterator<ProjectedSpan<int, [](int const& n) { return &n; }>>);
static_assert(
    not HasIterator<ProjectedSpan<int const, [](int const& n) { return &n; }>>);
static_assert(
    not HasConstIterator<ProjectedSpan<int, [](int& n) { return &n; }>>);
static_assert(HasIterator<ProjectedSpan<int, [](int& n) { return &n; }>>);

static_assert(std::random_access_iterator<typename ProjectedSpan<
                  int, [](int const& n) { return &n; }>::const_iterator>);
static_assert(
    std::random_access_iterator<typename ProjectedSpan<int, [](int const& n) {
      return &n;
    }>::const_reverse_iterator>);
static_assert(std::random_access_iterator<
              ProjectedSpan<int, [](int& n) { return &n; }>::iterator>);
static_assert(std::random_access_iterator<
              ProjectedSpan<int, [](int& n) { return &n; }>::reverse_iterator>);

NTH_TEST("ProjectedSpan/ContainerConstruction") {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<int, [](int const& n) { return &n; }> span(a);
  NTH_EXPECT(span.size() == size_t{5}) NTH_ELSE { return; }
  NTH_EXPECT(*span.front() == 1);
  NTH_EXPECT(*span.back() == 5);
  NTH_EXPECT(span >>=
             ElementsAreSequentially(PointsTo(1), PointsTo(2), PointsTo(3),
                                     PointsTo(4), PointsTo(5)));

  std::vector<int> v{5, 4, 3, 2, 1};
  span = v;
  NTH_EXPECT(span.size() == size_t{5}) NTH_ELSE { return; }
  NTH_EXPECT(*span.front() == 5);
  NTH_EXPECT(*span.back() == 1);
  NTH_EXPECT(span >>=
             ElementsAreSequentially(PointsTo(5), PointsTo(4), PointsTo(3),
                                     PointsTo(2), PointsTo(1)));
}

NTH_TEST("ProjectedSpan/IteratorPairConstruction") {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<int, [](int const& n) { return &n; }> span(a.begin(), a.end());
  NTH_EXPECT(span.size() == size_t{5}) NTH_ELSE { return; }
  NTH_EXPECT(*span.front() == 1);
  NTH_EXPECT(*span.back() == 5);
  NTH_EXPECT(span >>=
             ElementsAreSequentially(PointsTo(1), PointsTo(2), PointsTo(3),
                                     PointsTo(4), PointsTo(5)));
}

NTH_TEST("ProjectedSpan/RemoveEnds") {
  std::array<int, 5> a{1, 2, 3, 4, 5};
  ProjectedSpan<int, [](int const& n) { return &n; }> span(a);
  span.remove_prefix(1);
  NTH_EXPECT(span.size() == size_t{4}) NTH_ELSE { return; }
  NTH_EXPECT(*span.front() == 2);
  NTH_EXPECT(*span.back() == 5);
  NTH_EXPECT(span >>= ElementsAreSequentially(PointsTo(2), PointsTo(3),
                                              PointsTo(4), PointsTo(5)));
  span.remove_suffix(1);
  NTH_EXPECT(span.size() == size_t{3}) NTH_ELSE { return; }
  NTH_EXPECT(*span.front() == 2);
  NTH_EXPECT(*span.back() == 4);
  NTH_EXPECT(span >>=
             ElementsAreSequentially(PointsTo(2), PointsTo(3), PointsTo(4)));
}

NTH_TEST("ProjectedSpan/Empty") {
  ProjectedSpan<int, [](int const& n) { return &n; }> span;
  NTH_EXPECT(span.empty());
  std::array<int, 1> a{1};
  span = a;
  NTH_EXPECT(not span.empty());
  span.remove_prefix(1);
  NTH_EXPECT(span.empty());
}

NTH_TEST("ProjectedSpan/Data") {
  std::array<int, 1> a{1};
  ProjectedSpan<int, [](int const& n) { return &n; }> span(a);
  NTH_EXPECT(span.data() == a.data());
}

NTH_TEST("ProjectedSpan/CArray") {
  int a[5] = {1, 2, 3, 4, 5};
  ProjectedSpan<int, [](int const& n) { return &n; }> span(a);
  NTH_EXPECT(span.size() == size_t{5});
}

NTH_TEST("ProjectedSpan/DecltypeAuto") {
  std::pair<int, int> a[3] = {{1, 2}, {3, 4}, {5, 6}};
  ProjectedSpan<std::pair<int, int>,
                [](std::pair<int, int> const& p) -> int const& {
                  return p.first;
                }>
      span(a);
  NTH_EXPECT(&span.front() == &a[0].first);
  NTH_EXPECT(&span.back() == &a[2].first);
  NTH_EXPECT(&span[1] == &a[1].first);
}

NTH_TEST("ProjectedSpan/MutableAccess") {
  std::pair<int, int> a[3] = {{1, 2}, {3, 4}, {5, 6}};
  ProjectedSpan<std::pair<int, int>,
                [](std::pair<int, int>& p) -> int& { return p.first; }>
      span(a);
  NTH_EXPECT(&span.front() == &a[0].first);
  NTH_EXPECT(&span.back() == &a[2].first);
  NTH_EXPECT(&span[1] == &a[1].first);
  span[1] = 30;
  NTH_EXPECT(a[1].first == 30);
}

NTH_TEST("ProjectedSpan/MutableAndImmutableAccess") {
  static constexpr auto Proj = [](auto& p) -> decltype(auto) {
    return (p.first);
  };
  std::pair<int, int> a[3] = {{1, 2}, {3, 4}, {5, 6}};
  ProjectedSpan<std::pair<int, int>, Proj> m(a);
  ProjectedSpan<std::pair<int, int> const, Proj> c(a);
  NTH_EXPECT(&m.front() == &a[0].first);
  NTH_EXPECT(&m.back() == &a[2].first);
  NTH_EXPECT(&m[1] == &a[1].first);

  NTH_EXPECT(&c.front() == &a[0].first);
  NTH_EXPECT(&c.back() == &a[2].first);
  NTH_EXPECT(&c[1] == &a[1].first);

  m[1] = 30;
  NTH_EXPECT(m[1] == 30);

  NTH_EXPECT(c >>= ElementsAreSequentially(1, 30, 5));
  NTH_EXPECT(m >>= ElementsAreSequentially(1, 30, 5));
  NTH_EXPECT(a >>= ElementsAreSequentially(std::pair(1, 2), std::pair(30, 4),
                                           std::pair(5, 6)));
}

}  // namespace
}  // namespace nth
