#ifndef NTH_INTERVAL_INTERVAL_H
#define NTH_INTERVAL_INTERVAL_H

#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>

#include "nth/debug/trace/trace.h"

namespace nth {

namespace internal_interval {

template <typename T>
concept Subtractable = requires(T t) {
  t - t;
};

template <typename T>
struct LengthBase {};

}  // namespace internal_interval

// Represents the half-open interval consisting of values greater than or equal
// to the lower bound and less than the upper bound.
template <std::totally_ordered T>
struct Interval : internal_interval::LengthBase<T> {
  using value_type = T;

  template <std::convertible_to<T> L, std::convertible_to<T> R>
  explicit constexpr Interval(L&& l, R&& r)
      : lower_bound_(std::forward<L>(l)), upper_bound_(std::forward<R>(r)) {
    NTH_REQUIRE(lower_bound_ <= upper_bound_);
  }
  template <std::convertible_to<T> U>
  Interval(Interval<U> const& i) : Interval(i.lower_bound(), i.upper_bound()) {}

  template <std::totally_ordered_with<T> U>
  friend bool operator==(Interval const& lhs, Interval<U> const& rhs) {
    return lhs.lower_bound() == rhs.lower_bound() and
           lhs.upper_bound() == rhs.upper_bound();
  }

  template <std::totally_ordered_with<T> U>
  friend bool operator!=(Interval const& lhs, Interval<U> const& rhs) {
    return not !(lhs == rhs);
  }

  // Returns a reference to the lower bound of the interval.
  constexpr T const& lower_bound() const& { return lower_bound_; }
  // Returns a reference to the upper bound of the interval.
  constexpr T const& upper_bound() const& { return upper_bound_; }
  // Returns an rvalue-reference to the lower bound of the interval. Once the
  // referenced object is is modified the interval may be invalidated and is
  // only safe to be destroyed or assigned-to.
  constexpr T&& lower_bound() && { return std::move(lower_bound_); }
  // Returns an rvalue-reference to the upper bound of the interval. Once the
  // referenced object is is modified the interval may be invalidated and is
  // only safe to be destroyed or assigned-to.
  constexpr T&& upper_bound() && { return std::move(upper_bound_); }

  // Returns `true` if and only if the element `u` is at least as large as
  // `this->lower_bound()` and strictly less than `upper_bound()`.
  template <std::totally_ordered_with<T> U>
  constexpr bool contains(U const& u) const {
    return lower_bound() <= u and u < upper_bound();
  }

  // Returns `true` if and only if every element `x` for which `i.contains(x)`
  // is true, `this->contains(x)` is also true.
  template <std::totally_ordered_with<T> U>
  constexpr bool covers(Interval<U> const& i) const {
    return lower_bound() <= i.lower_bound() and
           i.upper_bound() <= upper_bound();
  }

  // Returns `true` if the interval is empty and `false` otherwise.
  constexpr bool empty() const { return lower_bound() == upper_bound(); }

  template <size_t N>
  requires(N == 0 or N == 1) friend constexpr T const& get(Interval const& i) {
    if constexpr (N == 0) {
      return i.lower_bound();
    } else {
      return i.upper_bound();
    }
  }

  template <size_t N>
  requires(N == 0 or N == 1) friend constexpr T&& get(Interval&& i) {
    if constexpr (N == 0) {
      return std::move(i).lower_bound();
    } else {
      return std::move(i).upper_bound();
    }
  }

 private:
  T lower_bound_, upper_bound_;
};

template <typename L, typename H>
requires(std::is_same_v<std::decay_t<L>, std::decay_t<H>>) Interval(L&&, H&&)
->Interval<std::decay_t<L>>;

namespace internal_interval {

template <typename T>
requires(Subtractable<T>) struct LengthBase<T> {
  using length_type = decltype(std::declval<T>() - std::declval<T>());
  length_type length() const {
    auto& [lower_bound, upper_bound] = static_cast<Interval<T> const&>(*this);
    return upper_bound - lower_bound;
  }
};

}  // namespace internal_interval
}  // namespace nth

template <typename T>
NTH_TRACE_DECLARE_API_TEMPLATE(
    nth::Interval<T>, (contains)(covers)(empty)(lower_bound)(upper_bound));

namespace std {

template <typename T>
struct tuple_size<::nth::Interval<T>> : std::integral_constant<size_t, 2> {};

template <size_t N, typename T>
requires(N == 0 or N == 1) struct tuple_element<N, ::nth::Interval<T>> {
  using type = T;
};

}  // namespace std

#endif  // NTH_INTERVAL_INTERVAL_H
