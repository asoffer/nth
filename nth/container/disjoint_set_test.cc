#include "nth/container/disjoint_set.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("disjoint_set/empty") {
  disjoint_set<int> set;
  NTH_EXPECT(set.empty());
  NTH_EXPECT(set.size() == 0u);
}

NTH_TEST("disjoint_set/insert") {
  disjoint_set<int> set;
  set.insert(1);
  set.insert(2);
  set.insert(3);
  NTH_EXPECT(not set.empty());
  NTH_EXPECT(set.size() == 3u);
  int m = 2, n = 3;
  set.insert(m);
  set.insert(n);
  NTH_EXPECT(not set.empty());
  NTH_EXPECT(set.size() == 3u);
}

NTH_TEST("disjoint_set/initializer_list") {
  disjoint_set<int> set{1, 2, 3, 1, 2};
  NTH_EXPECT(not set.empty());
  NTH_EXPECT(set.size() == 3u);
}

NTH_TEST("disjoint_set/representative") {
  disjoint_set<int> set{1, 2, 3};
  auto h = set.find(1);
  NTH_EXPECT(h == set.representative(h));
  NTH_EXPECT(*h == 1);

  h = set.find(2);
  NTH_EXPECT(h == set.representative(h));
  NTH_EXPECT(*h == 2);

  h = set.find(3);
  NTH_EXPECT(h == set.representative(h));
  NTH_EXPECT(*h == 3);
}

NTH_TEST("disjoint_set/join") {
  disjoint_set<int> set{1, 2, 3, 4};

  for (int i = 1; i <= 4; ++i) {
    NTH_EXPECT(set.representative(set.find(i)) == set.find_representative(i));
  }

  auto h1 = set.find(1);
  auto h2 = set.find(2);
  set.join(h1, h2);

  for (int i = 1; i <= 4; ++i) {
    NTH_EXPECT(set.representative(set.find(i)) == set.find_representative(i));
  }

  NTH_EXPECT(set.find_representative(1) == set.find_representative(2));
  NTH_EXPECT(set.find_representative(1) != set.find_representative(3));
  NTH_EXPECT(set.find_representative(1) != set.find_representative(4));
  NTH_EXPECT(set.find_representative(3) != set.find_representative(4));
  NTH_EXPECT(set.find_representative(3) == set.find(3));
  NTH_EXPECT(set.find_representative(4) == set.find(4));
  NTH_EXPECT(*h1 == 1 or *h1 == 2);
  NTH_EXPECT(*h2 == 1 or *h2 == 2);

  auto h3 = set.find(3);
  auto h4 = set.find(4);
  set.join(h3, h4);

  for (int i = 1; i <= 4; ++i) {
    NTH_EXPECT(set.representative(set.find(i)) == set.find_representative(i));
  }

  NTH_EXPECT(set.find_representative(1) == set.find_representative(2));
  NTH_EXPECT(set.find_representative(1) != set.find_representative(3));
  NTH_EXPECT(set.find_representative(1) != set.find_representative(4));
  NTH_EXPECT(set.find_representative(3) == set.find_representative(4));
  NTH_EXPECT(*h3 == 3 or *h3 == 4);
  NTH_EXPECT(*h4 == 3 or *h4 == 4);

  set.join(set.find(1), set.find(3));

  for (int i = 1; i <= 4; ++i) {
    NTH_EXPECT(set.representative(set.find(i)) == set.find_representative(i));
  }

  NTH_EXPECT(set.find_representative(1) == set.find_representative(2));
  NTH_EXPECT(set.find_representative(1) == set.find_representative(3));
  NTH_EXPECT(set.find_representative(1) == set.find_representative(4));
}

}  // namespace
}  // namespace nth
