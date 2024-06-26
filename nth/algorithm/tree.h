#ifndef NTH_ALGORITHM_TREE_H
#define NTH_ALGORITHM_TREE_H

#include <iostream>
#include <cstddef>
#include <span>
#include <variant>

#include "nth/algorithm/internal/tree.h"
#include "nth/base/core.h"
#include "nth/container/stack.h"
#include "nth/debug/unreachable.h"

namespace nth {

// This header contains algorithms for tree traversal.
// The primary function to use is `traverse_tree`, which accepts three
// parameters. The first is a tag indicating if the traversal is `nth::preorder`
// or `nth::postorder` (at the moment post-order traversal is not supported).
// The second parameter is a reference to a traverser which is responsible for
// dictating how the tree is traversed. The third argument is a
// `tree_traversal_stack` which should be initialized with the nodes needing to
// be traversed. On each traversed node in the tree, `NthTraverseTreeNode` will
// be invoked with the traverser, the node, and a reference to the
// `tree_traversal_stack`. Users must implement the `NthTraverseTreeNode`,
// pushing onto the `tree_traversal_stack`, any nodes they wish to visit. Nodes
// are visited in a depth-first manner. Specifically, the last node pushed onto
// the stack will be visited first.
//
// There are several optional functions traversal implementers may make
// available. If `NthTraverseTreeEnterSubtree` is invocable with the traverser,
// it will be called just before entering any non-empty subtree. Similarly, if
// `NthTraverseTreeExitSubtree` is invocable with the traverser, it will be
// called just after exiting any non-empty subtree. Lastly, if
// `NthTraverseTreeEnterLastChild` is invocable with the traverser, it will be
// called just before traversing the last of any collection of children of a
// node. If a node has exactly one child, the function will be called before
// visiting this one child.
//
// TODO: Support postorder traversal.
struct preorder_traversal_tag {};
inline constexpr preorder_traversal_tag preorder;
struct postorder_traversal_tag {};
inline constexpr postorder_traversal_tag postorder;

// Forward declarations of symbols defined below.
template <typename>
struct tree_traversal_stack;

template <int&..., typename Traverser, typename NodeType>
void traverse_tree(preorder_traversal_tag, Traverser&,
                   tree_traversal_stack<NodeType>);

// Implementation

template <typename T>
struct tree_traversal_stack {
  using value_type       = T;
  tree_traversal_stack() = default;

  [[nodiscard]] constexpr bool empty() const { return stack_.empty(); }

  void push(value_type const& v) { stack_.emplace(v); }
  void push(value_type&& v) { stack_.emplace(NTH_MOVE(v)); }
  void emplace(auto&&... args) {
    stack_.emplace(std::in_place_index<0>, NTH_FWD(args)...);
  }

 private:
  template <int&..., typename Traverser, typename NodeType>
  friend void traverse_tree(preorder_traversal_tag, Traverser&,
                            tree_traversal_stack<NodeType>);

  stack<std::variant<internal_algorithm::enter_subtree_tag,
                     internal_algorithm::enter_last_child_tag,
                     internal_algorithm::exit_subtree_tag, value_type>>
      stack_;
};

template <int&..., typename Traverser, typename NodeType>
void traverse_tree(preorder_traversal_tag, Traverser& traverser,
                   tree_traversal_stack<NodeType> stack) {
  static constexpr bool HasEnterSubtree = requires {
    NthTraverseTreeEnterSubtree(traverser);
  };
  static constexpr bool HasExitSubtree = requires {
    NthTraverseTreeExitSubtree(traverser);
  };
  static constexpr bool HasEnterLastChild = requires {
    NthTraverseTreeEnterLastChild(traverser);
  };

  while (not stack.empty()) {
    auto const& entry = stack.stack_.top();
    switch (entry.index()) {
      case 0:
        if constexpr (HasEnterSubtree) {
          stack.stack_.pop();
          NthTraverseTreeEnterSubtree(traverser);
          break;
        } else {
          NTH_UNREACHABLE();
        }
      case 1:
        if constexpr (HasEnterLastChild) {
          stack.stack_.pop();
          NthTraverseTreeEnterLastChild(traverser);
        } else {
          NTH_UNREACHABLE();
        }
        break;
      case 2:
        if constexpr (HasExitSubtree) {
          stack.stack_.pop();
          NthTraverseTreeExitSubtree(traverser);
          break;
        } else {
          NTH_UNREACHABLE();
        }
      default: {
        NodeType node = NTH_MOVE(std::get<3>(stack.stack_.top()));
        if constexpr (HasExitSubtree) {
          stack.stack_.top() = internal_algorithm::exit_subtree_tag{};
          if constexpr (HasEnterLastChild) {
            stack.stack_.push(internal_algorithm::enter_last_child_tag{});
          }
        } else if constexpr (HasEnterLastChild) {
          stack.stack_.top() = internal_algorithm::enter_last_child_tag{};
        } else {
          stack.stack_.pop();
        }

        size_t previous_size = stack.stack_.size();
        NthTraverseTreeNode(traverser, node, stack);
        size_t child_count = stack.stack_.size() - previous_size;
        if (child_count == 0) {
          stack.stack_.pop(HasExitSubtree + HasEnterLastChild);
        } else {
          if constexpr (HasEnterLastChild) {
            std::span prefixed_children =
                stack.stack_.top_span(child_count + 1);
            auto temp            = NTH_MOVE(prefixed_children[0]);
            prefixed_children[0] = NTH_MOVE(prefixed_children[1]);
            prefixed_children[1] = NTH_MOVE(temp);
          }
          if constexpr (HasEnterSubtree) {
            stack.stack_.push(internal_algorithm::enter_subtree_tag{});
          }
        }
      } break;
    }
  }
}

}  // namespace nth

#endif  // NTH_ALGORITHM_TREE_H
