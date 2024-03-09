#include "nth/container/flat_forest.h"

#include "nth/debug/property/property.h"
#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("flat_forest/default-construction") {
  flat_forest<std::string> f;
  NTH_EXPECT(f.empty());
  NTH_EXPECT(f.size() == 0u);
}

NTH_TEST("flat_forest/append_leaf") {
  flat_forest<std::string> f;
  auto index = f.append_leaf("leaf");
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == 1u);
  NTH_EXPECT(f[index] == "leaf");

  flat_forest<std::string> const cf = f;
  NTH_EXPECT(not cf.empty());
  NTH_EXPECT(cf.size() == 1u);
  NTH_EXPECT(cf[index] == "leaf");
}

NTH_TEST("flat_forest/append_leaf") {
  flat_forest<std::string> f;
  auto index = f.append_leaf("leaf");
  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == 1u);
  NTH_EXPECT(f[index] == "leaf");

  flat_forest<std::string> const cf = f;
  NTH_EXPECT(not cf.empty());
  NTH_EXPECT(cf.size() == 1u);
  NTH_EXPECT(cf[index] == "leaf");
}

NTH_TEST("flat_forest/append_ancestor") {
  flat_forest<std::string> f;
  auto a  = f.append_leaf("a");
  auto b  = f.append_leaf("b");
  auto c  = f.append_leaf("c");
  auto bc = f.append_ancestor(b, "bc");

  NTH_EXPECT(not f.empty());
  NTH_EXPECT(f.size() == 4u);
  NTH_EXPECT(f[a] == "a");
  NTH_EXPECT(f[b] == "b");
  NTH_EXPECT(f[c] == "c");
  NTH_EXPECT(f[bc] == "bc");

  flat_forest<std::string> const cf = f;
  NTH_EXPECT(not cf.empty());
  NTH_EXPECT(cf.size() == 4u);
  NTH_EXPECT(cf[a] == "a");
  NTH_EXPECT(cf[b] == "b");
  NTH_EXPECT(cf[c] == "c");
  NTH_EXPECT(cf[bc] == "bc");
}

NTH_TEST("flat_forest/entry/operator*") {
  flat_forest<std::string> f;
  auto a  = f.append_leaf("a");
  auto b  = f.append_leaf("b");
  auto c  = f.append_leaf("c");
  auto bc = f.append_ancestor(b, "bc");

  NTH_EXPECT(*f.entry(a) == "a");
  NTH_EXPECT(*f.entry(b) == "b");
  NTH_EXPECT(*f.entry(c) == "c");
  NTH_EXPECT(*f.entry(bc) == "bc");

  NTH_EXPECT(*f.centry(a) == "a");
  NTH_EXPECT(*f.centry(b) == "b");
  NTH_EXPECT(*f.centry(c) == "c");
  NTH_EXPECT(*f.centry(bc) == "bc");

  flat_forest<std::string> const cf = f;

  NTH_EXPECT(*cf.entry(a) == "a");
  NTH_EXPECT(*cf.entry(b) == "b");
  NTH_EXPECT(*cf.entry(c) == "c");
  NTH_EXPECT(*cf.entry(bc) == "bc");
}

NTH_TEST("flat_forest/entry/subtree") {
  flat_forest<std::string> f;
  auto a  = f.append_leaf("a");
  auto b  = f.append_leaf("b");
  auto c  = f.append_leaf("c");
  auto bc = f.append_ancestor(b, "bc");

  NTH_EXPECT(f.entry(a).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.entry(b).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.entry(c).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.entry(bc).subtree(post_order).size() == 3u);

  NTH_EXPECT(f.entry(a).subtree_size() == 1u);
  NTH_EXPECT(f.entry(b).subtree_size() == 1u);
  NTH_EXPECT(f.entry(c).subtree_size() == 1u);
  NTH_EXPECT(f.entry(bc).subtree_size() == 3u);

  NTH_EXPECT(f.centry(a).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.centry(b).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.centry(c).subtree(post_order).size() == 1u);
  NTH_EXPECT(f.centry(bc).subtree(post_order).size() == 3u);

  NTH_EXPECT(f.centry(a).subtree_size() == 1u);
  NTH_EXPECT(f.centry(b).subtree_size() == 1u);
  NTH_EXPECT(f.centry(c).subtree_size() == 1u);
  NTH_EXPECT(f.centry(bc).subtree_size() == 3u);

  flat_forest<std::string> const cf = f;

  NTH_EXPECT(cf.entry(a).subtree(post_order).size() == 1u);
  NTH_EXPECT(cf.entry(b).subtree(post_order).size() == 1u);
  NTH_EXPECT(cf.entry(c).subtree(post_order).size() == 1u);
  NTH_EXPECT(cf.entry(bc).subtree(post_order).size() == 3u);

  NTH_EXPECT(cf.entry(a).subtree_size() == 1u);
  NTH_EXPECT(cf.entry(b).subtree_size() == 1u);
  NTH_EXPECT(cf.entry(c).subtree_size() == 1u);
  NTH_EXPECT(cf.entry(bc).subtree_size() == 3u);
}

NTH_TEST("flat_forest/entry/descendants") {
  flat_forest<std::string> f;
  auto a  = f.append_leaf("a");
  auto b  = f.append_leaf("b");
  auto c  = f.append_leaf("c");
  auto bc = f.append_ancestor(b, "bc");

  NTH_EXPECT(f.entry(a).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.entry(b).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.entry(c).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.entry(bc).descendants(post_order).size() == 2u);

  NTH_EXPECT(f.centry(a).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.centry(b).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.centry(c).descendants(post_order).size() == 0u);
  NTH_EXPECT(f.centry(bc).descendants(post_order).size() == 2u);

  flat_forest<std::string> const cf = f;

  NTH_EXPECT(cf.entry(a).descendants(post_order).size() == 0u);
  NTH_EXPECT(cf.entry(b).descendants(post_order).size() == 0u);
  NTH_EXPECT(cf.entry(c).descendants(post_order).size() == 0u);
  NTH_EXPECT(cf.entry(bc).descendants(post_order).size() == 2u);
}

NTH_TEST("flat_forest/subtree/iteration") {
  flat_forest<std::string> f;
  auto a = f.append_leaf("a");
  f.append_leaf("b");
  f.append_ancestor(a, "ab");
  auto c = f.append_leaf("c");
  f.append_leaf("d");
  auto cd = f.append_ancestor(c, "cd");
  f.append_ancestor(a, "abcd");
  NTH_EXPECT(f.entry(cd).subtree(post_order) >>= debug::ElementsAreSequentially(
                 std::string("c"), std::string("d"), std::string("cd")));

  auto const cf = f;
  NTH_EXPECT(cf.entry(cd).subtree(post_order) >>=
             debug::ElementsAreSequentially(std::string("c"), std::string("d"),
                                            std::string("cd")));
}

NTH_TEST("flat_forest/sibling/iteration") {
  flat_forest<std::string> f;
  auto a = f.append_leaf("a");
  f.append_leaf("b");
  f.append_ancestor(a, "ab");
  auto c = f.append_leaf("c");
  f.append_leaf("d");
  auto cd   = f.append_ancestor(c, "cd");
  auto abcd = f.append_ancestor(a, "abcd");

  NTH_EXPECT(f.entry(abcd).children() >>= debug::ElementsAreSequentially(
                 std::string("cd"), std::string("ab")));
  NTH_EXPECT(f.entry(cd).children() >>= debug::ElementsAreSequentially(
                 std::string("d"), std::string("c")));

  auto const cf = f;
  NTH_EXPECT(cf.entry(abcd).children() >>= debug::ElementsAreSequentially(
                 std::string("cd"), std::string("ab")));
  NTH_EXPECT(cf.entry(cd).children() >>= debug::ElementsAreSequentially(
                 std::string("d"), std::string("c")));
}

NTH_TEST("flat_forest/roots/iteration") {
  flat_forest<std::string> f;
  f.append_leaf("a");
  auto b = f.append_leaf("b");
  f.append_leaf("c");
  f.append_ancestor(b, "bc");
  f.append_leaf("d");
  NTH_EXPECT(f.roots() >>= debug::ElementsAreSequentially(
                 std::string("d"), std::string("bc"), std::string("a")));

  NTH_EXPECT(f.croots() >>= debug::ElementsAreSequentially(
                 std::string("d"), std::string("bc"), std::string("a")));
  auto cf = f;

  NTH_EXPECT(f.roots() >>= debug::ElementsAreSequentially(
                 std::string("d"), std::string("bc"), std::string("a")));
}

NTH_TEST("flat_forest/roots/iteration/empty") {
  flat_forest<std::string> f;
  NTH_EXPECT(f.roots() >>= debug::ElementsAreSequentially());

  NTH_EXPECT(f.croots() >>= debug::ElementsAreSequentially());
  auto cf = f;

  NTH_EXPECT(f.roots() >>= debug::ElementsAreSequentially());
}

}  // namespace
}  // namespace nth
