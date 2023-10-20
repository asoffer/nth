#ifndef NTH_CONTAINER_INTERVAL_MAP_H
#define NTH_CONTAINER_INTERVAL_MAP_H

#include <concepts>
#include <iostream>
#include <utility>

#include "absl/container/btree_map.h"
#include "nth/container/interval.h"
#include "nth/container/interval_set.h"
#include "nth/debug/debug.h"

namespace nth {
namespace internal_interval_map {

template <typename K>
struct Cmp {
  constexpr bool operator()(::nth::interval<K> const& lhs,
                            ::nth::interval<K> const& rhs) const {
    if (lhs.lower_bound() < rhs.lower_bound()) { return true; }
    if (rhs.lower_bound() < lhs.lower_bound()) { return false; }
    return lhs.upper_bound() < rhs.upper_bound();
  }
};
}  // namespace internal_interval_map

// Represents an association of keys of type `K` with associated mapped values
// of type `M`, by storing a mapping from `nth::interval<K>` to `M` in an
// ordered associative container. The mapping is useful when ranges of values
// commonly have equal mapped values.
template <std::totally_ordered K, std::equality_comparable M>
struct interval_map {
  using key_type      = K;
  using interval_type = ::nth::interval<key_type>;
  using mapped_type   = M;

  explicit constexpr interval_map() = default;

  // Returns `true` if the interval set is empty and `false` otherwise.
  constexpr bool empty() const { return intervals_.empty(); }

  // Returns `true` if and only if the element `v` is contained in at one of the
  // intervals in the set.
  template <std::totally_ordered_with<key_type> T>
  constexpr bool contains(T const& v) const;

  // Returns `true` if and only if the interval `i` is completely covered by
  // some interval in the set.
  template <std::totally_ordered_with<key_type> T>
  constexpr bool covers(::nth::interval<T> const& i) const;

  // Returns `true` if and only if every interval in `is` is completely covered
  // by some interval in the set.
  template <std::totally_ordered_with<key_type> T>
  constexpr bool covers(::nth::interval_set<T> const& is) const;

  // After invocation, the keys in the map will be the union of those that were
  // in the map previously or the elements contained in `i`. Their associated
  // values will be `v` if the key is contained in `i` and the value it was
  // previously otherwise.
  template <std::totally_ordered_with<key_type> T,
            std::equality_comparable_with<mapped_type> V>
  void insert_or_assign(::nth::interval<T> const& i, V&& v);

  // If `k` is contained in the map, returns a pointer to the mapped range
  // containing `k`. Returns `nullptr` otherwise. The pointer is guaranteed to
  // remain valid until the next mutation of the map.
  interval_type const* key_range(key_type const& k) const;

  // If `k` is contained in the map, returns the corresponding mapped value.
  // Behavior is undefined otherwise. The reference is guaranteed to remain
  // valid until the next mutation of the map.
  mapped_type const& at(key_type const& k) const;

  // If `k` is contained in the map, returns a pointer to a pair consisting
  // of the widest interval containing the key mapped to the same value, and the
  // corresponding mapped value. Returns `nullptr` otherwise. The pointer is
  // guaranteed to remain valid until the next mutation of the map.
  std::pair<interval_type const, mapped_type> const* mapped_range(
      key_type const& k) const;

  // Returns a view into the intervals present in the interval map along with
  // their corresponding mapped values in increasing order. The view is valid
  // until the next non-const member function is invoked on this `interval_map`.
  auto const& mapped_intervals() const { return intervals_; }

  friend void NthPrint(auto& p, auto& f, interval_map const& is) {
    if (is.empty()) {
      p.write("{}");
      return;
    }
    std::string_view separator = "{";
    for (auto const& [k, m] : is.intervals_) {
      p.write(std::exchange(separator, ", "));
      f(p, k);
      p.write(" => ");
      f(p, m);
    }
    p.write("}");
  }

 private:
  using map_type   = absl::btree_map<interval_type, mapped_type,
                                  internal_interval_map::Cmp<key_type>>;
  using iterator       = typename map_type::iterator;
  using const_iterator = typename map_type::const_iterator;
  using value_type     = typename map_type::value_type;

  // Returns an iterator referencing first interval of the range from `b` to `e`
  // whose lower bound begins strictly after the lower bound of `i`. Returns `e`
  // if all elements of the range start before the lower bound of `i`, and `b`
  // if none of them do.
  static iterator FirstStartingAfter(iterator b, iterator e,
                                     key_type const& k) {
    return std::partition_point(b, e, [&](value_type const& entry) {
      return entry.first.lower_bound() <= k;
    });
  }
  static iterator FirstStartingNotBefore(iterator b, iterator e,
                                         key_type const& k) {
    return std::partition_point(b, e, [&](value_type const& entry) {
      return entry.first.lower_bound() < k;
    });
  }

  static const_iterator RangeContaining(const_iterator b, const_iterator e,
                                        key_type const& k) {
    auto iter = std::partition_point(b, e, [&](value_type const& entry) {
      return entry.first.upper_bound() < k;
    });
    if (iter == e) { return e; }
    return iter->first.lower_bound() <= k and k < iter->first.upper_bound()
               ? iter
               : e;
  }

  template <std::totally_ordered_with<key_type> T>
  constexpr bool covers_impl(const_iterator& it,
                             ::nth::interval<T> const& i) const;

  map_type intervals_;
};

template <std::totally_ordered K, std::equality_comparable M>
template <std::totally_ordered_with<K> T>
constexpr bool interval_map<K, M>::contains(T const& k) const {
  return key_range(k) != nullptr;
}

template <std::totally_ordered K, std::equality_comparable M>
template <std::totally_ordered_with<K> T>
constexpr bool interval_map<K, M>::covers_impl(
    const_iterator& it, ::nth::interval<T> const& i) const {
  auto l = RangeContaining(it, intervals_.end(), i.lower_bound());
  if (l == intervals_.end()) { return false; }
  auto end_iter =
      std::partition_point(l, intervals_.end(), [&](value_type const& entry) {
        return entry.first.lower_bound() <= i.upper_bound();
      });
  auto r = std::next(l);
  while (r != end_iter) {
    if (l->first.upper_bound() != r->first.lower_bound()) { return false; }
    l = std::exchange(r, std::next(r));
  }
  it = end_iter;
  return true;
}

template <std::totally_ordered K, std::equality_comparable M>
template <std::totally_ordered_with<K> T>
constexpr bool interval_map<K, M>::covers(::nth::interval<T> const& i) const {
  auto iter = intervals_.begin();
  return covers_impl(iter, i);
}

template <std::totally_ordered K, std::equality_comparable M>
template <std::totally_ordered_with<K> T>
constexpr bool interval_map<K, M>::covers(interval_set<T> const& is) const {
  auto iter = intervals_.begin();
  for (auto const& i : is.intervals()) {
    if (not covers_impl(iter, i)) { return false; }
  }
  return true;
}

template <std::totally_ordered K, std::equality_comparable M>
template <std::totally_ordered_with<K> T, std::equality_comparable_with<M> V>
void interval_map<K, M>::insert_or_assign(::nth::interval<T> const& i, V&& v) {
  if (intervals_.empty()) {
    intervals_.emplace(i, std::forward<V>(v));
    return;
  }

  iterator inserted_iter;
  auto start_iter =
      FirstStartingAfter(intervals_.begin(), intervals_.end(), i.lower_bound());
  if (start_iter == intervals_.end()) {
    inserted_iter = intervals_.emplace(i, std::forward<V>(v)).first;
  } else if (start_iter->first.upper_bound() > i.upper_bound()) {
    if (start_iter->first.lower_bound() > i.upper_bound()) {
      inserted_iter =
          intervals_.emplace_hint(start_iter, i, std::forward<V>(v));
    } else if (start_iter->second == v) {
      auto handle = intervals_.extract(start_iter);
      handle.key().set_lower_bound(i.lower_bound());
      auto result   = intervals_.insert(std::move(handle));
      inserted_iter = result.position;
    } else if (start_iter->first.lower_bound() == i.upper_bound()) {
      inserted_iter =
          intervals_.emplace_hint(start_iter, i, std::forward<V>(v));
    } else {
      auto handle = intervals_.extract(start_iter);
      handle.key().set_lower_bound(i.upper_bound());
      auto iter     = intervals_.insert(std::move(handle)).position;
      inserted_iter = intervals_.emplace_hint(iter, i, std::forward<V>(v));
    }
  } else {
    auto end_iter =
        FirstStartingNotBefore(start_iter, intervals_.end(), i.upper_bound());
    auto iter = intervals_.erase(start_iter, end_iter);
    if (iter != intervals_.end() and iter->second == v) {
      auto handle = intervals_.extract(iter);
      handle.key().set_lower_bound(i.lower_bound());
      inserted_iter = intervals_.insert(std::move(handle)).position;
    } else {
      inserted_iter = intervals_.emplace_hint(iter, i, std::forward<V>(v));
    }
  }

  // Check if it overlaps with the previous interval.
  if (inserted_iter == intervals_.begin()) { return; }
  auto prev_iter = std::prev(inserted_iter);
  if (prev_iter->first.upper_bound() < inserted_iter->first.lower_bound()) {
    return;
  }
  if (prev_iter->second == inserted_iter->second) {
    auto u      = inserted_iter->first.upper_bound();
    prev_iter   = std::prev(intervals_.erase(inserted_iter));
    auto handle = intervals_.extract(prev_iter);
    handle.key().set_upper_bound(std::move(u));
    if (not handle.key().empty()) { intervals_.insert(std::move(handle)); }
  } else {
    auto handle = intervals_.extract(prev_iter);
    handle.key().set_upper_bound(i.lower_bound());
    if (not handle.key().empty()) { intervals_.insert(std::move(handle)); }
  }
}
template <std::totally_ordered K, std::equality_comparable M>
::nth::interval<K> const* interval_map<K, M>::key_range(
    key_type const& k) const {
  interval_type const* result;
  NTH_ENSURE((v.debug), not result or result->contains(k));
  auto iter     = RangeContaining(intervals_.begin(), intervals_.end(), k);
  return result = (iter == intervals_.end()) ? nullptr : &iter->first;
}

template <std::totally_ordered K, std::equality_comparable M>
M const& interval_map<K, M>::at(key_type const& k) const {
  auto iter = RangeContaining(intervals_.begin(), intervals_.end(), k);
  NTH_REQUIRE((v.harden), iter != intervals_.end());
  return iter->second;
}

template <std::totally_ordered K, std::equality_comparable M>
std::pair<::nth::interval<K> const, M> const* interval_map<K, M>::mapped_range(
    key_type const& k) const {
  std::pair<interval_type const, mapped_type> const* result;
  NTH_ENSURE((v.debug), not result or result->first.contains(k));
  auto iter     = RangeContaining(intervals_.begin(), intervals_.end(), k);
  return result = (iter == intervals_.end()) ? nullptr : &*iter;
}

}  // namespace nth

template <typename K, typename M>
NTH_TRACE_DECLARE_API_TEMPLATE((nth::interval_map<K, M>),
                               (contains)(covers)(empty)(mapped_intervals));

#endif  // NTH_CONTAINER_INTERVAL_MAP_H
