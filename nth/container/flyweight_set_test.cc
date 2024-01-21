#include "nth/container/flyweight_set.h"

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::debug::ElementsAreSequentially;

NTH_TEST("flyweight_set/default-construction") {
  flyweight_set<std::string> f;
  NTH_EXPECT(f.empty());
  NTH_EXPECT(f.size() == size_t{0});
}

NTH_TEST("flyweight_set/insert") {
  flyweight_set<std::string> f;

  auto [p1, inserted1] = f.insert("abc");
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{1});

  auto [p2, inserted2] = f.insert("def");
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});

  auto [p3, inserted3] = f.insert("abc");
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{2});

  NTH_EXPECT(inserted1);
  NTH_EXPECT(inserted2);
  NTH_EXPECT(not inserted3);

  NTH_EXPECT(p1 != p2);
  NTH_EXPECT(p1 == p3);

  NTH_EXPECT(f >>=
             ElementsAreSequentially(std::string("abc"), std::string("def")));
}

NTH_TEST("flyweight_set/list-initialization") {
  flyweight_set<std::string> f{"a", "b", "c", "a", "c", "f"};

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(f >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("f")));
}

NTH_TEST("flyweight_set/clear") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});
  f.clear();
  NTH_EXPECT(f.empty());
  NTH_EXPECT(f.size() == size_t{0});
}

NTH_TEST("flyweight_set/copy-construction") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  flyweight_set<std::string> g = f;

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});

  NTH_EXPECT(f >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
  NTH_EXPECT(g >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
}

NTH_TEST("flyweight_set/move-construction") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  flyweight_set<std::string> g = std::move(f);

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});
  NTH_EXPECT(g >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
  NTH_EXPECT(g.find("a") != g.end());
}

NTH_TEST("flyweight_set/copy-assignment") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  flyweight_set<std::string> g;
  g = f;

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == size_t{4});

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});

  NTH_EXPECT(f >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
  NTH_EXPECT(g >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
}

NTH_TEST("flyweight_set/move-assignment") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  flyweight_set<std::string> g;
  g = std::move(f);

  NTH_EXPECT(not g.empty());
  NTH_EXPECT(g.size() == size_t{4});
  NTH_EXPECT(g >>= ElementsAreSequentially(std::string("a"), std::string("b"),
                                           std::string("c"), std::string("d")));
}

NTH_TEST("flyweight_set/iterators") {
  flyweight_set<std::string> f{"a", "b", "c", "d", "c", "b"};
  NTH_EXPECT(std::vector(f.begin(), f.end()) >>=
             ElementsAreSequentially(std::string("a"), std::string("b"),
                                     std::string("c"), std::string("d")));
  NTH_EXPECT(std::vector(f.cbegin(), f.cend()) >>=
             ElementsAreSequentially(std::string("a"), std::string("b"),
                                     std::string("c"), std::string("d")));
  NTH_EXPECT(std::vector(f.rbegin(), f.rend()) >>=
             ElementsAreSequentially(std::string("d"), std::string("c"),
                                     std::string("b"), std::string("a")));
  NTH_EXPECT(std::vector(f.crbegin(), f.crend()) >>=
             ElementsAreSequentially(std::string("d"), std::string("c"),
                                     std::string("b"), std::string("a")));
}

NTH_TEST("flyweight_set/const-access") {
  flyweight_set<std::string> const f{"a", "b", "c", "d", "a"};
  NTH_EXPECT(f.front() == "a");
  NTH_EXPECT(f.back() == "d");
}

NTH_TEST("flyweight_set/find") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  NTH_EXPECT(f.find("x") == f.end());
  NTH_EXPECT(f.find("a") == f.begin());
  NTH_EXPECT(f.find("d") == std::prev(f.end()));
}

NTH_TEST("flyweight_set/index") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  NTH_EXPECT(f.index("x") == f.end_index());
  NTH_EXPECT(f.index("a") == size_t{0});
  NTH_EXPECT(f.index("d") == size_t{3});
}

NTH_TEST("flyweight_set/stress-test") {
  flyweight_set<size_t> f;
  for (size_t i = 0; i < 1000; ++i) {
    f.insert(i);
    for (size_t j : f) { NTH_EXPECT(f.index(j) == j); }
  }
}

NTH_TEST("flyweight_set/from-index") {
  flyweight_set<std::string> f{"a", "b", "c", "d"};
  NTH_EXPECT(f.from_index(0) == "a");
  NTH_EXPECT(f.from_index(1) == "b");
  NTH_EXPECT(f.from_index(2) == "c");
  NTH_EXPECT(f.from_index(3) == "d");
}

}  // namespace
}  // namespace nth
