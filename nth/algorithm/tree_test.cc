#include "nth/algorithm/tree.h"

#include <vector>

#include "nth/test/raw/test.h"

namespace {

struct Tree {
  std::vector<int> indices;
};

constexpr int enter_subtree    = -1;
constexpr int exit_subtree     = -2;
constexpr int enter_last_child = -3;

template <bool HasEnterSubtree, bool HasExitSubtree, bool HasEnterLastChild>
struct Traverser {
  Tree const& tree;

  std::vector<int> nodes;

  friend void NthTraverseTreeNode(Traverser& t, int n,
                                  nth::tree_traversal_stack<int>& stack) {
    t.nodes.push_back(n);
    for (size_t i = 0; i < t.tree.indices.size(); ++i) {
      if (t.tree.indices[i] == n) { stack.push(i); }
    }
  }

  friend void NthTraverseTreeEnterSubtree(Traverser& t) requires(
      HasEnterSubtree) {
    t.nodes.push_back(enter_subtree);
  }

  friend void NthTraverseTreeExitSubtree(Traverser& t) requires(
      HasExitSubtree) {
    t.nodes.push_back(exit_subtree);
  }

  friend void NthTraverseTreeEnterLastChild(Traverser& t) requires(
      HasEnterLastChild) {
    t.nodes.push_back(enter_last_child);
  }
};

void EmptyTraversal() {
  Tree tree = {.indices = {}};
  Traverser<false, false, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{});
}

void TraverseLinePreorder() {
  Tree tree = {.indices = {-1, 0, 1, 2}};
  Traverser<false, false, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{0, 1, 2, 3});
}
void TraverseMultiplePreorder() {
  Tree tree = {.indices = {-1, 0, 1, 2}};
  Traverser<false, false, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(1);
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{0, 1, 2, 3, 1, 2, 3});
}

// The structure of the tree used for the rest of the tests here is given
// diagramatically by:
//
// 0 -> 1 -> 2 -> 3
// |    |    |
// v    v    v
// 4    5    6
// |
// v
// 7 -> 8, 9, 10, 11

void TraverseComplexPreorder() {
  Tree tree = {.indices = {-1, 0, 1, 2, 0, 1, 2, 4, 7, 7, 7, 7}};
  Traverser<false, false, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes ==
                      std::vector<int>{0, 4, 7, 11, 10, 9, 8, 1, 5, 2, 6, 3});
}

void TraverseComplexPreorderHasEnterSubtree() {
  Tree tree = {.indices = {-1, 0, 1, 2, 0, 1, 2, 4, 7, 7, 7, 7}};
  Traverser<true, false, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(
      t.nodes == std::vector<int>{0, enter_subtree, 4, enter_subtree, 7,
                                  enter_subtree, 11, 10, 9, 8, 1, enter_subtree,
                                  5, 2, enter_subtree, 6, 3});
}

void TraverseComplexPreorderHasExitSubtree() {
  Tree tree = {.indices = {-1, 0, 1, 2, 0, 1, 2, 4, 7, 7, 7, 7}};
  Traverser<false, true, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{0, 4, 7, 11, 10, 9, 8,exit_subtree,
                                                  exit_subtree,  1,
                                                  5, 2, 6, 3, exit_subtree,
                                                  exit_subtree, exit_subtree});
}

void TraverseComplexPreorderHasEnterAndExitSubtree() {
  Tree tree = {.indices = {-1, 0, 1, 2, 0, 1, 2, 4, 7, 7, 7, 7}};
  Traverser<true, true, false> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);
  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{0,
                                                  enter_subtree,
                                                  4,
                                                  enter_subtree,
                                                  7,
                                                  enter_subtree,
                                                  11,
                                                  10,
                                                  9,
                                                  8,
                                                  exit_subtree,
                                                  exit_subtree,
                                                  1,
                                                  enter_subtree,
                                                  5,
                                                  2,
                                                  enter_subtree,
                                                  6,
                                                  3,
                                                  exit_subtree,
                                                  exit_subtree,
                                                  exit_subtree});
}

void TraverseComplexPreorderHasEnterLastChild() {
  Tree tree = {.indices = {-1, 0, 1, 2, 0, 1, 2, 4, 7, 7, 7, 7}};
  Traverser<true, true, true> t{.tree = tree};
  nth::tree_traversal_stack<int> stack;
  stack.push(0);
  nth::traverse_tree(nth::preorder, t, stack);

  NTH_RAW_TEST_ASSERT(t.nodes == std::vector<int>{0,
                                                  enter_subtree,
                                                  4,
                                                  enter_subtree,
                                                  enter_last_child,
                                                  7,
                                                  enter_subtree,
                                                  11,
                                                  10,
                                                  9,
                                                  enter_last_child,
                                                  8,
                                                  exit_subtree,
                                                  exit_subtree,
                                                  enter_last_child,
                                                  1,
                                                  enter_subtree,
                                                  5,
                                                  enter_last_child,
                                                  2,
                                                  enter_subtree,
                                                  6,
                                                  enter_last_child,
                                                  3,
                                                  exit_subtree,
                                                  exit_subtree,
                                                  exit_subtree});
}

}  // namespace

int main() {
  EmptyTraversal();
  TraverseLinePreorder();
  TraverseMultiplePreorder();
  TraverseComplexPreorder();
  TraverseComplexPreorderHasEnterSubtree();
  TraverseComplexPreorderHasExitSubtree();
  TraverseComplexPreorderHasEnterAndExitSubtree();
  TraverseComplexPreorderHasEnterLastChild();
}
