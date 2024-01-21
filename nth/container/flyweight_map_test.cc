#include "nth/container/flyweight_map.h"

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::debug::ElementsAreSequentially;

NTH_TEST("flyweight_map/default-construction") {
  flyweight_map<int, std::string> f;
  NTH_EXPECT(f.empty());
  NTH_EXPECT(f.size() == size_t{0});
}

NTH_TEST("flyweight_map/try-emplace") {
  flyweight_map<int, std::string> f;

  auto [p1, inserted1] = f.try_emplace(1, 1, 'x');
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{1});

  auto [p2, inserted2] = f.try_emplace(2, 1, 'y');
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});

  auto [p3, inserted3] = f.try_emplace(1, 1, 'z');
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});

  NTH_EXPECT(inserted1);
  NTH_EXPECT(inserted2);
  NTH_EXPECT(not inserted3);

  NTH_EXPECT(p1 != p2);
  NTH_EXPECT(p1 == p3);

  NTH_EXPECT(
      f >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "x"),
                                    std::pair<int const, std::string>(2, "y")));
}

NTH_TEST("flyweight_map/operator[]") {
  flyweight_map<int, std::string> f;

  f[1];
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{1});
  NTH_EXPECT(*f.find(1) == std::pair<int const, std::string>(1, ""));

  f[1] = "a";
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{1});
  NTH_EXPECT(*f.find(1) == std::pair<int const, std::string>(1, "a"));

  f[2] = "b";
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});
  NTH_EXPECT(*f.find(2) == std::pair<int const, std::string>(2, "b"));

  f[1] = "c";
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});
  NTH_EXPECT(*f.find(1) == std::pair<int const, std::string>(1, "c"));

  NTH_EXPECT(
      f >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "c"),
                                    std::pair<int const, std::string>(2, "b")));
}

NTH_TEST("flyweight_map/list-initialization") {
  flyweight_map<int, std::string> f{{3, "a"}, {2, "b"}, {1, "c"},
                                    {2, "d"}, {3, "e"}, {4, "f"}};

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(
      f >>= ElementsAreSequentially(std::pair<int const, std::string>(3, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(1, "c"),
                                    std::pair<int const, std::string>(4, "f")));
}

NTH_TEST("flyweight_map/clear") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});
  f.clear();
  NTH_EXPECT(f.empty());
  NTH_EXPECT(f.size() == size_t{0});
}

NTH_TEST("flyweight_map/copy-construction") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  flyweight_map<int, std::string> g = f;

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});

  NTH_EXPECT(
      f >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));
  NTH_EXPECT(
      g >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));

  // Ensure that the members of `g` are not referencing into `f`
  f[1] = "A";
  NTH_EXPECT(*g.find(1) == std::pair<int const, std::string>(1, "a"));
}

NTH_TEST("flyweight_map/move-construction") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  flyweight_map<int, std::string> g = std::move(f);

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});
  NTH_EXPECT(
      g >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));
  NTH_EXPECT(g.find(1) != g.end());
}

NTH_TEST("flyweight_map/copy-assignment") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  flyweight_map<int, std::string> g;
  g = f;

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});

  NTH_EXPECT(
      f >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));
  NTH_EXPECT(
      g >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));

  f[1] = "A";
  NTH_EXPECT(*g.find(1) == std::pair<int const, std::string>(1, "a"));
}

NTH_TEST("flyweight_map/move-assignment") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  flyweight_map<int, std::string> g;
  g = std::move(f);

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});
  NTH_EXPECT(
      g >>= ElementsAreSequentially(std::pair<int const, std::string>(1, "a"),
                                    std::pair<int const, std::string>(2, "b"),
                                    std::pair<int const, std::string>(3, "c"),
                                    std::pair<int const, std::string>(4, "d")));
}

NTH_TEST("flyweight_map/iterators") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"},
                                    {4, "d"}, {1, "e"}, {2, "f"}};
  NTH_EXPECT(std::vector(f.begin(), f.end()) >>= ElementsAreSequentially(
                 std::pair<int const, std::string>(1, "a"),
                 std::pair<int const, std::string>(2, "b"),
                 std::pair<int const, std::string>(3, "c"),
                 std::pair<int const, std::string>(4, "d")));
  NTH_EXPECT(std::vector(f.cbegin(), f.cend()) >>= ElementsAreSequentially(
                 std::pair<int const, std::string>(1, "a"),
                 std::pair<int const, std::string>(2, "b"),
                 std::pair<int const, std::string>(3, "c"),
                 std::pair<int const, std::string>(4, "d")));
  NTH_EXPECT(std::vector(f.rbegin(), f.rend()) >>= ElementsAreSequentially(
                 std::pair<int const, std::string>(4, "d"),
                 std::pair<int const, std::string>(3, "c"),
                 std::pair<int const, std::string>(2, "b"),
                 std::pair<int const, std::string>(1, "a")));
  NTH_EXPECT(std::vector(f.crbegin(), f.crend()) >>= ElementsAreSequentially(
                 std::pair<int const, std::string>(4, "d"),
                 std::pair<int const, std::string>(3, "c"),
                 std::pair<int const, std::string>(2, "b"),
                 std::pair<int const, std::string>(1, "a")));
}

NTH_TEST("flyweight_map/const-access") {
  flyweight_map<int, std::string> const f{
      {1, "a"}, {2, "b"}, {1, "c"}, {3, "d"}, {1, "e"}};
  NTH_EXPECT(f.front() == std::pair<int const, std::string>(1, "a"));
  NTH_EXPECT(f.back() == std::pair<int const, std::string>(3, "d"));
}

NTH_TEST("flyweight_map/access") {
  flyweight_map<int, std::string> f{
      {1, "a"}, {2, "b"}, {1, "c"}, {3, "d"}, {1, "e"}};
  NTH_EXPECT(f.front() == std::pair<int const, std::string>(1, "a"));
  NTH_EXPECT(f.back() == std::pair<int const, std::string>(3, "d"));

  f.front().second = "A";
  f.back().second  = "D";
  NTH_EXPECT(f.front() == std::pair<int const, std::string>(1, "A"));
  NTH_EXPECT(f.back() == std::pair<int const, std::string>(3, "D"));
}

NTH_TEST("flyweight_map/find") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  NTH_EXPECT(f.find(0) == f.end());
  NTH_EXPECT(f.find(1) == f.begin());
  NTH_EXPECT(*f.find(4) == std::pair<int const, std::string>(4, "d"));
  NTH_ASSERT(f.find(4) != f.end());
  NTH_EXPECT(std::distance(f.begin(), f.find(4)) == 3);
}

NTH_TEST("flyweight_map/from-index") {
  flyweight_map<int, std::string> f{{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}};
  NTH_EXPECT(f.from_index(0) == std::pair<int const, std::string>(1, "a"));
  NTH_EXPECT(f.from_index(1) == std::pair<int const, std::string>(2, "b"));
  NTH_EXPECT(f.from_index(2) == std::pair<int const, std::string>(3, "c"));
  NTH_EXPECT(f.from_index(3) == std::pair<int const, std::string>(4, "d"));

  auto const& cf = f;
  NTH_EXPECT(cf.from_index(0) == std::pair<int const, std::string>(1, "a"));
  NTH_EXPECT(cf.from_index(1) == std::pair<int const, std::string>(2, "b"));
  NTH_EXPECT(cf.from_index(2) == std::pair<int const, std::string>(3, "c"));
  NTH_EXPECT(cf.from_index(3) == std::pair<int const, std::string>(4, "d"));
}

NTH_TEST("flyweight_map/stress-test") {
  flyweight_map<size_t, size_t> f;
  for (size_t i = 0; i < 1000; ++i) {
    f.try_emplace(i);
    for (auto [j, ignored] : f) { NTH_ASSERT(f.index(j) == j); }
  }
}

}  // namespace
}  // namespace nth
