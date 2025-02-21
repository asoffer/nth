#ifndef NTH_CONTAINER_INTERVAL_H
#define NTH_CONTAINER_INTERVAL_H

#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>

#include "nth/debug/debug.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"

namespace nth {

namespace internal_interval {

template <typename T>
concept Subtractable = requires(T t) { t - t; };

template <typename T>
struct LengthBase {};

}  // namespace internal_interval

// Represents the half-open interval consisting of values greater than or equal
// to the lower bound and less than the upper bound.
template <std::totally_ordered T>
struct interval : internal_interval::LengthBase<T> {
  using value_type = T;

  template <std::convertible_to<value_type> L,
            std::convertible_to<value_type> R>
  explicit constexpr interval(L&& l, R&& r)
      : lower_bound_(std::forward<L>(l)), upper_bound_(std::forward<R>(r)) {
    NTH_REQUIRE(lower_bound_ <= upper_bound_);
  }
  template <std::convertible_to<value_type> U>
  interval(interval<U> const& i) : interval(i.lower_bound(), i.upper_bound()) {}

  template <std::totally_ordered_with<value_type> U>
  friend bool operator==(interval const& lhs, interval<U> const& rhs) {
    return lhs.lower_bound() == rhs.lower_bound() and
           lhs.upper_bound() == rhs.upper_bound();
  }

  template <std::totally_ordered_with<value_type> U>
  friend bool operator!=(interval const& lhs, interval<U> const& rhs) {
    return not !(lhs == rhs);
  }

  // Returns a reference to the lower bound of the interval.
  constexpr value_type const& lower_bound() const& { return lower_bound_; }
  // Returns a reference to the upper bound of the interval.
  constexpr value_type const& upper_bound() const& { return upper_bound_; }
  // Returns an rvalue-reference to the lower bound of the interval. Once the
  // referenced object is is modified the interval may be invalidated and is
  // only safe to be destroyed or assigned-to.
  constexpr value_type&& lower_bound() && { return std::move(lower_bound_); }
  // Returns an rvalue-reference to the upper bound of the interval. Once the
  // referenced object is is modified the interval may be invalidated and is
  // only safe to be destroyed or assigned-to.
  constexpr value_type&& upper_bound() && { return std::move(upper_bound_); }

  // Sets the lower bound to the specified `value`. Behavior is undefined if
  // `value` does not compare less-than or equal to `upper_bound()`.
  template <std::totally_ordered_with<value_type> U>
  constexpr void set_lower_bound(U&& value) {
    NTH_REQUIRE(value <= upper_bound_);
    lower_bound_ = std::forward<U>(value);
  }

  // Sets the upper bound to the specified `value`. Behavior is undefined if
  // `value` does not compare greater-than or equal to `lower_bound()`.
  template <std::totally_ordered_with<value_type> U>
  constexpr void set_upper_bound(U&& value) {
    NTH_REQUIRE(lower_bound_ <= value);
    upper_bound_ = std::forward<U>(value);
  }

  // Returns `true` if and only if the element `u` is at least as large as
  // `this->lower_bound()` and strictly less than `upper_bound()`.
  template <std::totally_ordered_with<value_type> U>
  constexpr bool contains(U const& u) const {
    return lower_bound() <= u and u < upper_bound();
  }

  // Returns `true` if and only if every element `x` for which `i.contains(x)`
  // is true, `this->contains(x)` is also true.
  template <std::totally_ordered_with<value_type> U>
  constexpr bool covers(interval<U> const& i) const {
    return lower_bound() <= i.lower_bound() and
           i.upper_bound() <= upper_bound();
  }

  // Returns `std::nullopt` if the intervals do not intersect. Otherwise,
  // returns the intersection of the intervals.
  std::optional<interval> intersection(interval const& i) const {
    value_type const& l = std::max(lower_bound(), i.lower_bound());
    value_type const& u = std::min(upper_bound(), i.upper_bound());
    if (l < u) { return interval(l, u); }
    return std::nullopt;
  }

  // Returns `true` if the interval is empty and `false` otherwise.
  constexpr bool empty() const { return lower_bound() == upper_bound(); }

  template <size_t N>
    requires(N == 0 or N == 1)
  friend constexpr value_type const& get(interval const& i) {
    if constexpr (N == 0) {
      return i.lower_bound();
    } else {
      return i.upper_bound();
    }
  }

  template <size_t N>
    requires(N == 0 or N == 1)
  friend constexpr value_type&& get(interval&& i) {
    if constexpr (N == 0) {
      return std::move(i).lower_bound();
    } else {
      return std::move(i).upper_bound();
    }
  }

  friend void NthFormat(io::writer auto& w, auto& fmt, interval const& i) {
    nth::io::write_text(w, "[");
    nth::format(w, fmt, i.lower_bound());
    nth::io::write_text(w, ", ");
    nth::format(w, fmt, i.upper_bound());
    nth::io::write_text(w, ")");
  }

 private:
  value_type lower_bound_, upper_bound_;
};

template <typename L, typename H>
  requires(std::is_same_v<std::decay_t<L>, std::decay_t<H>>)
interval(L&&, H&&) -> interval<std::decay_t<L>>;

namespace internal_interval {

template <typename T>
  requires(Subtractable<T>)
struct LengthBase<T> {
  using length_type = decltype(std::declval<T>() - std::declval<T>());
  length_type length() const {
    auto& [lower_bound, upper_bound] = static_cast<interval<T> const&>(*this);
    return upper_bound - lower_bound;
  }
};

}  // namespace internal_interval
}  // namespace nth

namespace std {

template <typename T>
struct tuple_size<::nth::interval<T>> : std::integral_constant<size_t, 2> {};

template <size_t N, typename T>
  requires(N == 0 or N == 1)
struct tuple_element<N, ::nth::interval<T>> {
  using type = T;
};

}  // namespace std

#endif  // NTH_CONTAINER_INTERVAL_H
