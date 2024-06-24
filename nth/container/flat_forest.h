#ifndef NTH_CONTAINER_FLAT_FOREST_H
#define NTH_CONTAINER_FLAT_FOREST_H

#include <vector>

#include "nth/utility/iterator_range.h"

namespace nth {
namespace internal_flat_forest {

struct sentinel_iterator {};

}  // namespace internal_flat_forest

// A `flat_forest` is a tree-like data structure that is designed to be fast to
// append nodes, for a post-order traversals, and for iterating through children
// of a node in reverse. The name contains "forest" rather than "tree" because
// there is no guarantee that there is a unique root. The name contains "flat"
// to indicate that all nodes are stored in contiguous storage. Broadly speaking
// appending nodes will be fast, but deletions may be expensive.
template <typename T>
struct flat_forest {
  using value_type = T;

  // `index_type` provides a stable accessor for entries contained in the
  // `flat_forest`, via `operator[]`. Values of type `index_type` are not
  // invalidated by append operations, but may be invalidated by deletions or
  // insertions. Indices remain valid after a `flat_forest` has been moved (for
  // the moved-to object, not the moved-from object). They may also be used to
  // index into the corresponding nodes of a copied `flat_forest`.
  struct index_type;

  // `entry_type` and `const_entry_type` (colloquially, "entries") represent
  // accessors to a node in the forest, including not just its associated value,
  // but also the tree-structure (e.g., a node's children). Values of these
  // types may be invalidated on any append, insert, or deletion operation.
  // Entries my be accessed from an index via the `entry` member functions.
  struct entry_type;
  struct const_entry_type;

  // `post_order_iterator` and `const_post_order_iterator` iterate through
  // values in a post-order traversal.
  struct const_post_order_iterator;
  struct post_order_iterator;

  // `sibling_iterator` and `const_sibling_iterator` iterate sibling nodes
  // (i.e., nodes all sharing a common parent).
  struct sibling_iterator;
  struct const_sibling_iterator;

  // Returns the number of nodes in the forest.
  size_t size() const { return nodes_.size(); }

  // Returns `true` if there are no nodes in the forsest, and `false` otherwise.
  bool empty() const { return nodes_.empty(); }

  // Returns a reference to the `value_type` stored in the node referred to by
  // `index`.
  value_type const &operator[](index_type index) const;
  value_type &operator[](index_type index);

  // Returns a(n) `[const_]entry_type` referring to the node referred to by
  // `index`. Entry types are not stable identifiers, but can be used to access
  // the tree-structure of the `flat_forest`.
  const_entry_type entry(index_type index) const;
  entry_type entry(index_type index);
  const_entry_type centry(index_type index) const;

  // Appends a new leaf of type `value_type` to the forest as if constructed by
  // `value_type(std::forward<Args>(args)...)`.
  template <typename... Args>
  index_type append_leaf(
      Args &&...args) requires std::constructible_from<T, Args...>;

  // Appends a new node of type `value_type` to the forest as if constructed by
  // `value_type(std::forward<Args>(args)...)`. The node will be an ancestor of
  // all nodes appended to after the node referred to by `index`. If any such
  // node is an ancestor of nodes appended *before* the node referred to by
  // `index`, behavior is undefined. Becasue the descendants of a node are
  // always appended before the node itself, this implies that, amongst other
  // requirements, the node referred to by `index` must be a leaf.
  template <typename... Args>
  index_type append_ancestor(index_type index, Args &&...args) requires
      std::constructible_from<T, Args...>;

  // Returns an iterator range over all nodes that have no parent.
  auto roots();
  auto roots() const;
  auto croots() const;

 private:
  struct const_node_range;
  struct node_range;

  struct node_type {
    template <typename... Args>
    node_type(size_t subtree_size,
              Args &&...args) requires std::constructible_from<T, Args...>;

   private:
    friend flat_forest;
    friend entry_type;
    friend const_entry_type;
    friend post_order_iterator;
    friend const_post_order_iterator;
    friend sibling_iterator;
    friend const_sibling_iterator;

    size_t subtree_size_;
    value_type value_;
  };

  [[maybe_unused]] bool IsValidCutpoint(size_t index) const;

  std::vector<node_type> nodes_;
};

template <typename T>
struct flat_forest<T>::index_type {
 private:
  template <typename>
  friend struct flat_forest;

  constexpr index_type() = default;
  constexpr index_type(size_t n) : index_(n) {}
  size_t index_;
};

template <typename T>
template <typename... Args>
flat_forest<T>::index_type flat_forest<T>::append_leaf(
    Args &&...args) requires std::constructible_from<T, Args...> {
  index_type index(nodes_.size());
  nodes_.emplace_back(1, std::forward<Args>(args)...);
  return index;
}

template <typename T>
template <typename... Args>
flat_forest<T>::index_type flat_forest<T>::append_ancestor(
    index_type index,
    Args &&...args) requires std::constructible_from<T, Args...> {
  NTH_REQUIRE((debug), IsValidCutpoint(index.index_));
  index_type return_index(nodes_.size());
  nodes_.emplace_back(nodes_.size() - index.index_ + 1,
                      std::forward<Args>(args)...);
  return return_index;
}

template <typename T>
template <typename... Args>
flat_forest<T>::node_type::node_type(
    size_t subtree_size,
    Args &&...args) requires std::constructible_from<T, Args...>
    : subtree_size_(subtree_size), value_(std::forward<Args>(args)...) {}

struct post_order_traversal_t {};
inline constexpr post_order_traversal_t post_order;

template <typename T>
struct flat_forest<T>::const_entry_type {
  // Returns a reference to the `value_type` stored in this node.
  value_type const &operator*() const { return entry_->value_; }

  value_type *operator->() const { return std::addressof(entry_->value_); }

  // Returns the number of nodes in subtree corresponding to this node,
  // including the node itself.
  size_t subtree_size() const { return entry_->subtree_size_; }

  // Returns a range over all descendants of this node (excluding the node
  // itself). Iteration through the nodes will traverse the nodes in post order.
  auto descendants(post_order_traversal_t) const {
    return nth::iterator_range(
        const_post_order_iterator(entry_ - entry_->subtree_size_ + 1),
        const_post_order_iterator(entry_));
  }

  // Returns a range over all descendants of this node (including the node
  // itself). Iteration through the nodes will traverse the nodes in post order.
  auto subtree(post_order_traversal_t) const {
    return nth::iterator_range(
        const_post_order_iterator(entry_ - entry_->subtree_size_ + 1),
        const_post_order_iterator(entry_ + 1));
  }

  auto children() const {
    return nth::iterator_range(
        const_sibling_iterator(entry_ - 1, entry_->subtree_size_ - 1),
        internal_flat_forest::sentinel_iterator{});
  }

 protected:
  friend flat_forest<T>;
  explicit const_entry_type(node_type const *entry) : entry_(entry) {}
  node_type const *entry_;
};

template <typename T>
struct flat_forest<T>::entry_type {
  // Returns a reference to the `value_type` stored in this node.
  value_type &operator*() const { return entry_->value_; }

  value_type *operator->() const { return std::addressof(entry_->value_); }

  // Returns the number of nodes in subtree corresponding to this node,
  // including the node itself.
  size_t subtree_size() const { return entry_->subtree_size_; }

  // Returns a range over all descendants of this node (excluding the node
  // itself). Iteration through the nodes will traverse the nodes in post order.
  auto descendants(post_order_traversal_t) const {
    return nth::iterator_range(
        post_order_iterator(entry_ - entry_->subtree_size_ + 1),
        post_order_iterator(entry_));
  }

  // Returns a range over all descendants of this node (including the node
  // itself). Iteration through the nodes will traverse the nodes in post order.
  auto subtree(post_order_traversal_t) const {
    return nth::iterator_range(
        post_order_iterator(entry_ - entry_->subtree_size_ + 1),
        post_order_iterator(entry_ + 1));
  }

  auto children() const {
    return nth::iterator_range(
        sibling_iterator(entry_ - 1, entry_->subtree_size_ - 1),
        internal_flat_forest::sentinel_iterator{});
  }

 protected:
  friend flat_forest<T>;
  explicit entry_type(node_type *entry) : entry_(entry) {}
  node_type *entry_;
};

namespace internal_flat_forest {

template <typename EntryType, typename It>
struct iterator : EntryType {
 public:
  It &operator++() {
    self().increment();
    return self();
  }

  It operator++(int) {
    auto copy = clone();
    self().increment();
    return copy;
  }

  friend It operator+(It it, int n) {
    it.increment_by(n);
    return it;
  }

  friend It operator+(int n, It it) {
    it.increment_by(n);
    return it;
  }

  It &operator--() requires(requires(It &it) { it.decrement(); }) {
    self().decrement();
    return self();
  }

  It operator--(int) requires(requires(It &it) { it.decrement(); }) {
    auto copy = clone();
    self().decrement();
    return copy;
  }

  friend It operator-(It it, int n) requires(requires(It &it) {
    it.decrement_by(0);
  }) {
    it.decrement_by(n);
    return it;
  }

 protected:
  constexpr explicit iterator(auto const *ptr)
      : EntryType(const_cast<std::remove_cvref_t<decltype(*ptr)> *>(ptr)) {}

 private:
  It clone() const { return self(); }
  It &self() { return static_cast<It &>(*this); }
  It const &self() const { return static_cast<It const &>(*this); }
};

}  // namespace internal_flat_forest

template <typename T>
struct flat_forest<T>::const_post_order_iterator
    : internal_flat_forest::iterator<
          const_entry_type, flat_forest<T>::const_post_order_iterator> {
  friend ptrdiff_t operator-(const_post_order_iterator lhs,
                             const_post_order_iterator rhs) {
    return lhs.entry_ - rhs.entry_;
  }

  friend bool operator==(const_post_order_iterator lhs,
                         const_post_order_iterator rhs) {
    return lhs.entry_ == rhs.entry_;
  }

 private:
  friend internal_flat_forest::iterator<
      const_entry_type, flat_forest<T>::const_post_order_iterator>;
  friend flat_forest<T>::const_entry_type;

  void increment() { ++this->entry_; }
  void increment_by(int n) { this->entry_ += n; }
  void decrement() { --this->entry_; }
  void decrement_by(int n) { this->entry_ -= n; }

  explicit const_post_order_iterator(node_type const *ptr)
      : internal_flat_forest::iterator<const_entry_type,
                                       const_post_order_iterator>(ptr) {}
};

template <typename T>
struct flat_forest<T>::post_order_iterator
    : internal_flat_forest::iterator<entry_type,
                                     flat_forest<T>::post_order_iterator> {
  friend ptrdiff_t operator-(post_order_iterator lhs, post_order_iterator rhs) {
    return lhs.entry_ - rhs.entry_;
  }

  friend bool operator==(post_order_iterator lhs, post_order_iterator rhs) {
    return lhs.entry_ == rhs.entry_;
  }

 private:
  friend internal_flat_forest::iterator<entry_type,
                                        flat_forest<T>::post_order_iterator>;
  friend flat_forest<T>::entry_type;

  void increment() { ++this->entry_; }
  void increment_by(int n) { this->entry_ += n; }
  void decrement() { --this->entry_; }
  void decrement_by(int n) { this->entry_ -= n; }

  explicit post_order_iterator(node_type *ptr)
      : internal_flat_forest::iterator<entry_type, post_order_iterator>(ptr) {}
};

template <typename T>
struct flat_forest<T>::const_sibling_iterator
    : internal_flat_forest::iterator<const_entry_type,
                                     flat_forest<T>::const_sibling_iterator> {
  friend bool operator==(const_sibling_iterator lhs,
                         internal_flat_forest::sentinel_iterator) {
    return lhs.n_ == 0;
  }

 private:
  friend internal_flat_forest::iterator<const_entry_type,
                                        flat_forest<T>::const_sibling_iterator>;
  friend flat_forest<T>;
  friend flat_forest<T>::const_entry_type;

  void increment() {
    n_ -= this->entry_->subtree_size_;
    this->entry_ -= this->entry_->subtree_size_;
  }
  void increment_by(int n) {
    for (int i = 0; i < n; ++i) { ++n; }
  }

  explicit const_sibling_iterator(node_type const *ptr, size_t n)
      : internal_flat_forest::iterator<const_entry_type,
                                       const_sibling_iterator>(ptr),
        n_(n) {}
  size_t n_;
};

template <typename T>
struct flat_forest<T>::sibling_iterator
    : internal_flat_forest::iterator<entry_type,
                                     flat_forest<T>::sibling_iterator> {
  friend bool operator==(sibling_iterator lhs,
                         internal_flat_forest::sentinel_iterator) {
    return lhs.n_ == 0;
  }

 private:
  friend internal_flat_forest::iterator<entry_type,
                                        flat_forest<T>::sibling_iterator>;
  friend flat_forest<T>;
  friend flat_forest<T>::entry_type;

  void increment() {
    n_ -= this->entry_->subtree_size_;
    this->entry_ -= this->entry_->subtree_size_;
  }
  void increment_by(int n) {
    for (int i = 0; i < n; ++i) { ++n; }
  }

  explicit sibling_iterator(node_type *ptr, size_t n)
      : internal_flat_forest::iterator<entry_type, sibling_iterator>(ptr),
        n_(n) {}

  size_t n_;
};

template <typename T>
struct flat_forest<T>::const_node_range {
  explicit const_node_range(const_entry_type low, const_entry_type high)
      : low_(low), high_(high) {}

  const_post_order_iterator begin() const;
  const_post_order_iterator end() const;

 private:
  const_entry_type low_, high_;
};

template <typename T>
struct flat_forest<T>::node_range {
  explicit node_range(entry_type low, entry_type high)
      : low_(low), high_(high) {}

  post_order_iterator begin() const;
  post_order_iterator end() const;

 private:
  entry_type low_, high_;
};

template <typename T>
flat_forest<T>::value_type const &flat_forest<T>::operator[](
    index_type index) const {
  return nodes_[index.index_].value_;
}

template <typename T>
flat_forest<T>::value_type &flat_forest<T>::operator[](index_type index) {
  return nodes_[index.index_].value_;
}

template <typename T>
flat_forest<T>::const_entry_type flat_forest<T>::entry(index_type index) const {
  return const_entry_type(&nodes_[index.index_]);
}

template <typename T>
flat_forest<T>::entry_type flat_forest<T>::entry(index_type index) {
  return entry_type(&nodes_[index.index_]);
}

template <typename T>
flat_forest<T>::const_entry_type flat_forest<T>::centry(
    index_type index) const {
  return const_entry_type(&nodes_[index.index_]);
}

template <typename T>
bool flat_forest<T>::IsValidCutpoint(size_t index) const {
  NTH_REQUIRE((debug), not empty());
  size_t loc = nodes_.size();
  while (loc > index) { loc -= nodes_[loc - 1].subtree_size_; }
  return loc == index;
}

template <typename T>
auto flat_forest<T>::roots() {
  return nth::iterator_range(
      sibling_iterator(nodes_.empty() ? nullptr : &nodes_.back(),
                       nodes_.size()),
      internal_flat_forest::sentinel_iterator{});
}

template <typename T>
auto flat_forest<T>::roots() const {
  return nth::iterator_range(
      const_sibling_iterator(nodes_.empty() ? nullptr : &nodes_.back(),
                             nodes_.size()),
      internal_flat_forest::sentinel_iterator{});
}

template <typename T>
auto flat_forest<T>::croots() const {
  return nth::iterator_range(
      const_sibling_iterator(nodes_.empty() ? nullptr : &nodes_.back(),
                             nodes_.size()),
      internal_flat_forest::sentinel_iterator{});
}

}  // namespace nth

#endif  // NTH_CONTAINER_FLAT_FOREST_H
