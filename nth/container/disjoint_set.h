#ifndef NTH_CONTAINER_DISJOINT_SET_H
#define NTH_CONTAINER_DISJOINT_SET_H

#include <cstddef>
#include <initializer_list>
#include <utility>

#include "nth/container/flyweight_map.h"
#include "nth/debug/debug.h"
#include "nth/memory/buffer.h"

namespace nth {

template <typename T>
struct disjoint_set;

namespace internal_disjoint_set {

struct Entry {
  size_t size;
  // Working under the assumption that the size of an iterator is independent
  // of the size of the `flyweight_map`.
  buffer_sufficient_for<flyweight_map<int64_t, int64_t>::iterator> buffer;
};

struct passkey {
 private:
  passkey() = default;
  template <typename>
  friend struct ::nth::disjoint_set;
};

}  // namespace internal_disjoint_set

// Represents a collection of distinct values of type `T` that have been
// aggregated into disjoint subsets of the entire set. For each element, one can
// ask for a representative of the subset it belongs to. Any two elements
// belonging to the same subset will have the same representative.
template <typename T>
struct disjoint_set {
  using value_type = T;

  // Constructs an empty `disjoint_set`.
  constexpr disjoint_set() = default;

  // Constructs a `disjoint_set` containing the elements in `values`, each in a
  // separate subset.
  constexpr disjoint_set(std::initializer_list<value_type> values) {
    for (auto &v : values) { insert(v); }
  }

  struct handle {
    // Returns an empty handle.
    explicit constexpr handle() : ptr_(nullptr) {}

    explicit handle(
        flyweight_map<value_type, internal_disjoint_set::Entry>::iterator iter,
        internal_disjoint_set::passkey = {})
        : ptr_(&*iter) {}

    [[nodiscard]] bool empty() const { return ptr_ == nullptr; }

    // Returns a reference to the underlying element referred to by this handle.
    // Behavior is undefined if the handle is unpopulated.
    value_type const &operator*() const {
      NTH_REQUIRE((v.debug), ptr_ != nullptr);
      return ptr_->first;
    }

    friend bool operator==(handle, handle) = default;

    template <typename H>
    friend H AbslHashValue(H h, handle handle) {
      return H::combine(std::move(h), handle.ptr_);
    }

   private:
    friend disjoint_set;

    handle &parent() { return ptr_->second.buffer.template as<handle>(); }
    size_t &size() { return ptr_->second.size; }

    std::pair<value_type const, internal_disjoint_set::Entry> *ptr_;
  };
  static_assert(std::is_trivially_destructible_v<handle>);

  // Inserts the element `v` into the set if it was not already present in the
  // set. Returns a pair consisting of a handle to the inserted element, and a
  // bool indicating whether insertion took place. Notably, the returned handle
  // is for the element, not necessarily its representative.
  std::pair<handle, bool> insert(value_type const &v);
  std::pair<handle, bool> insert(value_type &&v);

  // Returns a handle to an element equivalent to `v` if an element equivalent
  // to `v` is present in the container. An empty handle is returned otherwise.
  handle find(value_type const &v);

  // Returns a handle to an element in the same subset as `v`. Behavior is
  // undefined if `v` is not present in the container.
  handle find_representative(value_type const &v);

  // Returns a representative handle referencing an element in the same subset
  // as `*h`. This function is idempotentent.
  handle representative(handle h);

  // After invocation, `a` and `b` will be contained in the same subset. Returns
  // a handle to a representative of the newly formed subset.
  handle join(handle a, handle b);

  // Returns whether or not the set is empty.
  bool empty() const { return entries_.empty(); }

  // Returns the number of elements in the set (not the number of subsets).
  size_t size() const { return entries_.size(); }

 private:
  handle representative_impl(handle h);

  flyweight_map<value_type, internal_disjoint_set::Entry> entries_;
};

// Implementation

template <typename T>
disjoint_set<T>::handle disjoint_set<T>::find(value_type const &v) {
  auto iter = entries_.find(v);
  if (iter == entries_.end()) { return handle(); }
  return handle(iter);
}

template <typename T>
disjoint_set<T>::handle disjoint_set<T>::find_representative(
    value_type const &v) {
  auto iter = entries_.find(v);
  if (iter == entries_.end()) { return handle(); }
  return representative(handle(iter));
}

template <typename T>
disjoint_set<T>::handle disjoint_set<T>::join(handle a, handle b) {
  a = representative(a);
  b = representative(b);
  if (a == b) { return a; }

  if (a.size() < b.size()) { std::swap(a, b); }

  b.parent() = a;
  a.size() += b.size();
  return a;
}

template <typename T>
std::pair<typename disjoint_set<T>::handle, bool> disjoint_set<T>::insert(
    value_type const &v) {
  auto [iter, inserted] =
      entries_.try_emplace(v, internal_disjoint_set::Entry{.size = 1});
  if (inserted) { iter->second.buffer.template construct<handle>(iter); }
  return std::make_pair(handle(iter), inserted);
}

template <typename T>
std::pair<typename disjoint_set<T>::handle, bool> disjoint_set<T>::insert(
    value_type &&v) {
  auto [iter, inserted] = entries_.try_emplace(
      std::move(v), internal_disjoint_set::Entry{.size = 1});
  if (inserted) { iter->second.buffer.template construct<handle>(iter); }
  return std::make_pair(handle(iter), inserted);
}

template <typename T>
typename disjoint_set<T>::handle disjoint_set<T>::representative(handle h) {
  if (h.empty()) { return h; }
  return representative_impl(h);
}

template <typename T>
typename disjoint_set<T>::handle disjoint_set<T>::representative_impl(
    handle h) {
  handle &parent = h.parent();
  if (parent == h) { return h; }
  return parent = representative_impl(parent);
}

}  // namespace nth

#endif  // NTH_CONTAINER_DISJOINT_SET_H
