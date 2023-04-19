#ifndef NTH_INTERVAL_INTERVAL_H
#define NTH_INTERVAL_INTERVAL_H

#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>

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
  template <std::convertible_to<T> L, std::convertible_to<T> R>
  explicit constexpr Interval(L&& l, R&& r)
      : lower_bound_(std::forward<L>(l)), upper_bound_(std::forward<R>(r)) {
    assert(lower_bound_ < upper_bound_);
  }

  constexpr T const& lower_bound() const& { return lower_bound_; }
  constexpr T&& lower_bound() && { return std::move(lower_bound_); }

  constexpr T const& upper_bound() const& { return upper_bound_; }
  constexpr T&& upper_bound() && { return std::move(upper_bound_); }

  // Returns `true` if and only if the element `u` is at least as large as
  // `this->lower_bound()` and strictly less than `upper_bound()`.
  template <std::totally_ordered_with<T> U>
  constexpr bool contains(U const& u) {
    return lower_bound() <= u and u < upper_bound();
  }

  // Returns `true` if the interval is empty and `false` otherwise.
  constexpr bool empty() const { return lower_bound() == upper_bound(); }

  template <size_t N>
  requires(N == 0 or N == 1) constexpr T const& get() const& {
    if constexpr (N == 0) {
      return lower_bound();
    } else {
      return upper_bound();
    }
  }

  template <size_t N>
  requires(N == 0 or N == 1) constexpr T&& get() && {
    if constexpr (N == 0) {
      return std::move(*this).lower_bound();
    } else {
      return std::move(*this).upper_bound();
    }
  }

 private:
  T lower_bound_, upper_bound_;
};

template <typename T>
Interval(T, T) -> Interval<T>;

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

namespace std {

template <typename T>
struct tuple_size<::nth::Interval<T>> : std::integral_constant<size_t, 2> {};

template <size_t N, typename T>
requires(N == 0 or N == 1) struct tuple_element<N, ::nth::Interval<T>> {
  using type = T;
};

}  // namespace std

#endif  // NTH_INTERVAL_INTERVAL_H