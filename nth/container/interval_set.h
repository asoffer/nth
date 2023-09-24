#ifndef NTH_CONTAINER_INTERVAL_SET_H
#define NTH_CONTAINER_INTERVAL_SET_H

#include <algorithm>
#include <concepts>
#include <span>
#include <vector>

#include "nth/container/interval.h"

namespace nth {

// Represents the half-open interval consisting of values greater than or equal
// to the lower bound and less than the upper bound.
template <std::totally_ordered T>
struct interval_set {
  using interval_type = interval<T>;
  using value_type    = T;

  explicit constexpr interval_set() = default;

  template <std::convertible_to<T> U>
  explicit constexpr interval_set(interval<U> const& interval)
      : intervals_(interval) {}

  template <std::convertible_to<T> U>
  explicit constexpr interval_set(interval<U>&& interval) {
    intervals_.push_back(std::move(interval));
  }

  // Returns `true` if the interval set is empty and `false` otherwise.
  constexpr bool empty() const { return intervals_.empty(); }

  // Returns `true` if and only if the element `u` is contained in at one of the
  // intervals in the set.
  template <std::totally_ordered_with<T> U>
  constexpr bool contains(U const& u) const;

  // Returns `true` if and only if the interval `i` is completely covered by
  // some interval in the set.
  template <std::totally_ordered_with<T> U>
  constexpr bool covers(interval<U> const& i) const;

  // Returns `true` if and only if every interval in `is` is completely covered
  // by some interval in the set.
  template <std::totally_ordered_with<T> U>
  constexpr bool covers(interval_set<U> const& is) const;

  // Returns the sum total length of all intervals contained in this interval
  // set.
  constexpr auto length() const
      requires(requires(interval_type const& i) { i.length(); }) {
    decltype(std::declval<interval<T> const&>().length()) result = {};
    for (auto const& i : intervals_) { result += i.length(); }
    return result;
  }

  template <std::totally_ordered_with<T> U>
  void insert(interval<U> const& i) {
    insert_hint(intervals_.begin(), i);
  }

  template <std::totally_ordered_with<T> U>
  interval_set& operator+=(interval_set<U> const& rhs);

  friend interval_set operator+(interval_set const& lhs,
                                interval_set const& rhs) {
    interval_set copy(lhs);
    return copy += rhs;
  }

  friend interval_set operator+(interval_set&& lhs, interval_set const& rhs) {
    return std::move(lhs += rhs);
  }

  friend interval_set operator+(interval_set const& lhs, interval_set&& rhs) {
    return std::move(rhs += lhs);
  }

  friend interval_set operator+(interval_set&& lhs, interval_set&& rhs) {
    return std::move(lhs += rhs);
  }

  // Returns a view into the intervals present in the interval set in increasing
  // order. The view is valid until the next non-const member function is
  // invoked on this `interval_set`.
  std::span<interval<T> const> intervals() const { return intervals_; }

  friend void NthPrint(auto& p, auto& f, interval_set const& is) {
    if (is.empty()) {
      p.write("{}");
      return;
    }
    std::string_view separator = "{";
    for (auto const& i : is.intervals_) {
      p.write(std::exchange(separator, ", "));
      f(p, i);
    }
    p.write("}");
  }

 private:
  using iterator = typename std::vector<interval<T>>::iterator;

  template <std::totally_ordered_with<T> U>
  iterator insert_hint(iterator iter, interval<U> const& i);

  std::vector<interval<T>> intervals_;
};

template <std::totally_ordered T>
interval_set(T&&, T&&) -> interval_set<std::decay_t<T>>;

template <std::totally_ordered T>
interval_set(interval<T> const&) -> interval_set<T>;

// Implementations below.

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool interval_set<T>::contains(U const& u) const {
  auto iter = std::partition_point(
      intervals_.begin(), intervals_.end(),
      [&](interval<T> const& interval) { return interval.lower_bound() <= u; });
  if (iter == intervals_.begin()) { return false; }
  return u < std::prev(iter)->upper_bound();
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool interval_set<T>::covers(interval<U> const& i) const {
  auto iter = std::partition_point(
      intervals_.begin(), intervals_.end(), [&](interval<T> const& interval) {
        return interval.lower_bound() <= i.lower_bound();
      });
  if (iter == intervals_.begin()) { return false; }
  return i.upper_bound() <= std::prev(iter)->upper_bound();
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool interval_set<T>::covers(interval_set<U> const& is) const {
  auto iter = intervals_.begin();
  for (auto const& i : is.intervals()) {
    auto new_iter = std::lower_bound(
        iter, intervals_.end(), [&](interval<T> const& interval) {
          return interval.lower_bound() < i.lower_bound();
        });
    if (iter == new_iter) { return false; }
    iter = std::prev(new_iter);
    if (iter->upper_bound() < i.upper_bound()) { return false; }
  }
  return true;
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
typename interval_set<T>::iterator interval_set<T>::insert_hint(
    iterator iter, interval<U> const& i) {
  auto lower_iter = std::partition_point(
      iter, intervals_.end(), [&](interval<T> const& interval) {
        return interval.upper_bound() < i.lower_bound();
      });

  if (lower_iter == intervals_.end()) {
    intervals_.push_back(i);
    return std::prev(intervals_.end());
  }

  auto upper_iter = std::partition_point(
      lower_iter, intervals_.end(), [&](interval<T> const& interval) {
        return interval.lower_bound() <= i.upper_bound();
      });

  if (upper_iter == lower_iter) {
    if (upper_iter->lower_bound() == i.upper_bound()) {
      *upper_iter = interval(i.lower_bound(), upper_iter->upper_bound());
      return upper_iter;
    } else {
      return intervals_.insert(lower_iter, i);
    }
  }
  --upper_iter;
  assert(lower_iter <= upper_iter);

  auto l      = std::min(lower_iter->lower_bound(), i.lower_bound());
  auto u      = std::max(upper_iter->upper_bound(), i.upper_bound());
  *lower_iter = interval(l, u);
  if (lower_iter != upper_iter) {
    intervals_.erase(std::next(lower_iter), std::next(upper_iter));
  }
  return lower_iter;
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
interval_set<T>& interval_set<T>::operator+=(interval_set<U> const& rhs) {
  auto start_iter = intervals_.begin();
  for (auto iter = rhs.intervals_.begin(); iter != rhs.intervals_.end();
       ++iter) {
    start_iter = insert_hint(start_iter, *iter);
  }
  return *this;
}

template <std::totally_ordered T>
interval_set<T> Union(interval_set<T> const& lhs, interval_set<T> const& rhs) {
  return lhs + rhs;
}

template <std::totally_ordered T>
interval_set<T> Union(interval_set<T> const& lhs, interval_set<T>&& rhs) {
  return lhs + std::move(rhs);
}

template <std::totally_ordered T>
interval_set<T> Union(interval_set<T>&& lhs, interval_set<T> const& rhs) {
  return std::move(lhs) + rhs;
}

template <std::totally_ordered T>
interval_set<T> Union(interval_set<T>&& lhs, interval_set<T>&& rhs) {
  return std::move(lhs) + std::move(rhs);
}

}  // namespace nth

template <typename T>
NTH_TRACE_DECLARE_API_TEMPLATE(nth::interval_set<T>,
                               (contains)(covers)(empty)(intervals)(length));

#endif  // NTH_CONTAINER_INTERVAL_SET_H
