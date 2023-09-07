#ifndef NTH_INTERVAL_INTERVAL_SET_H
#define NTH_INTERVAL_INTERVAL_SET_H

#include <algorithm>
#include <concepts>
#include <iostream>
#include <span>
#include <vector>

#include "nth/interval/interval.h"

namespace nth {

// Represents the half-open interval consisting of values greater than or equal
// to the lower bound and less than the upper bound.
template <std::totally_ordered T>
struct IntervalSet {
  using interval_type = Interval<T>;
  using value_type    = T;

  explicit constexpr IntervalSet() = default;

  template <std::convertible_to<T> U>
  explicit constexpr IntervalSet(Interval<U> const& interval)
      : intervals_(interval) {}

  template <std::convertible_to<T> U>
  explicit constexpr IntervalSet(Interval<U>&& interval) {
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
  constexpr bool covers(Interval<U> const& i) const;

  // Returns `true` if and only if every interval in `is` is completely covered
  // by some interval in the set.
  template <std::totally_ordered_with<T> U>
  constexpr bool covers(IntervalSet<U> const& is) const;

  // Returns the sum total length of all intervals contained in this interval
  // set.
  constexpr auto length() const
      requires(requires(interval_type const& i) { i.length(); }) {
    decltype(std::declval<Interval<T> const&>().length()) result = {};
    for (auto const& i : intervals_) { result += i.length(); }
    return result;
  }

  template <std::totally_ordered_with<T> U>
  void insert(Interval<U> const& i) {
    insert_hint(intervals_.begin(), i);
  }

  template <std::totally_ordered_with<T> U>
  IntervalSet& operator+=(IntervalSet<U> const& rhs);

  friend IntervalSet operator+(IntervalSet const& lhs, IntervalSet const& rhs) {
    IntervalSet copy(lhs);
    return copy += rhs;
  }

  friend IntervalSet operator+(IntervalSet&& lhs, IntervalSet const& rhs) {
    return std::move(lhs += rhs);
  }

  friend IntervalSet operator+(IntervalSet const& lhs, IntervalSet&& rhs) {
    return std::move(rhs += lhs);
  }

  friend IntervalSet operator+(IntervalSet&& lhs, IntervalSet&& rhs) {
    return std::move(lhs += rhs);
  }

  // Returns a view into the intervals present in the interval set in increasing
  // order. The view is valid until the next non-const member function is
  // invoked on this `IntervalSet`.
  std::span<Interval<T> const> intervals() const { return intervals_; }

 private:
  using iterator = typename std::vector<Interval<T>>::iterator;

  template <std::totally_ordered_with<T> U>
  iterator insert_hint(iterator iter, Interval<U> const& i);

  std::vector<Interval<T>> intervals_;
};

template <std::totally_ordered T>
IntervalSet(T&&, T&&) -> IntervalSet<std::decay_t<T>>;

template <std::totally_ordered T>
IntervalSet(Interval<T> const&) -> IntervalSet<T>;

// Implementations below.

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool IntervalSet<T>::contains(U const& u) const {
  auto iter = std::partition_point(
      intervals_.begin(), intervals_.end(),
      [&](Interval<T> const& interval) { return interval.lower_bound() <= u; });
  if (iter == intervals_.begin()) { return false; }
  return u < std::prev(iter)->upper_bound();
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool IntervalSet<T>::covers(Interval<U> const& i) const {
  auto iter = std::partition_point(
      intervals_.begin(), intervals_.end(), [&](Interval<T> const& interval) {
        return interval.lower_bound() <= i.lower_bound();
      });
  if (iter == intervals_.begin()) { return false; }
  return i.upper_bound() <= std::prev(iter)->upper_bound();
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
constexpr bool IntervalSet<T>::covers(IntervalSet<U> const& is) const {
  auto iter = intervals_.begin();
  for (auto const& i : is.intervals()) {
    auto new_iter = std::lower_bound(
        iter, intervals_.end(), [&](Interval<T> const& interval) {
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
typename IntervalSet<T>::iterator IntervalSet<T>::insert_hint(
    iterator iter, Interval<U> const& i) {
  auto lower_iter = std::partition_point(
      iter, intervals_.end(), [&](Interval<T> const& interval) {
        return interval.upper_bound() < i.lower_bound();
      });

  if (lower_iter == intervals_.end()) {
    intervals_.push_back(i);
    return std::prev(intervals_.end());
  }

  auto upper_iter = std::partition_point(
      lower_iter, intervals_.end(), [&](Interval<T> const& interval) {
        return interval.lower_bound() <= i.upper_bound();
      });

  if (upper_iter == lower_iter) {
    if (upper_iter->lower_bound() == i.upper_bound()) {
      *upper_iter = Interval(i.lower_bound(), upper_iter->upper_bound());
      return upper_iter;
    } else {
      return intervals_.insert(lower_iter, i);
    }
  }
  --upper_iter;
  assert(lower_iter <= upper_iter);

  auto l      = std::min(lower_iter->lower_bound(), i.lower_bound());
  auto u      = std::max(upper_iter->upper_bound(), i.upper_bound());
  *lower_iter = Interval(l, u);
  if (lower_iter != upper_iter) {
    intervals_.erase(std::next(lower_iter), std::next(upper_iter));
  }
  return lower_iter;
}

template <std::totally_ordered T>
template <std::totally_ordered_with<T> U>
IntervalSet<T>& IntervalSet<T>::operator+=(IntervalSet<U> const& rhs) {
  auto start_iter = intervals_.begin();
  for (auto iter = rhs.intervals_.begin(); iter != rhs.intervals_.end();
       ++iter) {
    start_iter = insert_hint(start_iter, *iter);
  }
  return *this;
}

template <std::totally_ordered T>
IntervalSet<T> Union(IntervalSet<T> const& lhs, IntervalSet<T> const& rhs) {
  return lhs + rhs;
}

template <std::totally_ordered T>
IntervalSet<T> Union(IntervalSet<T> const& lhs, IntervalSet<T>&& rhs) {
  return lhs + std::move(rhs);
}

template <std::totally_ordered T>
IntervalSet<T> Union(IntervalSet<T>&& lhs, IntervalSet<T> const& rhs) {
  return std::move(lhs) + rhs;
}

template <std::totally_ordered T>
IntervalSet<T> Union(IntervalSet<T>&& lhs, IntervalSet<T>&& rhs) {
  return std::move(lhs) + std::move(rhs);
}

}  // namespace nth

template <typename T>
NTH_TRACE_DECLARE_API_TEMPLATE(nth::IntervalSet<T>,
                               (contains)(covers)(empty)(intervals)(length));

#endif  // NTH_INTERVAL_INTERVAL_SET_H
