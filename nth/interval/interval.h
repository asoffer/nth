#ifndef NTH_INTERVAL_INTERVAL_H
#define NTH_INTERVAL_INTERVAL_H

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

template <std::totally_ordered T>
struct Interval : internal_interval::LengthBase<T> {
  template <std::convertible_to<T> L, std::convertible_to<T> R>
  explicit Interval(L&& l, R&& r)
      : start_(std::forward<L>(l)), end_(std::forward<R>(r)) {}

  T const& start() const& { return start_; }
  T&& start() && { return std::move(start_); }
  T const& end() const& { return end_; }
  T&& end() && { return std::move(end_); }

  template <size_t N>
  T const& get() const& requires(N == 0 or N == 1) {
    if constexpr (N == 0) {
      return start();
    } else {
      return end();
    }
  }
  template <size_t N>
      T&& get() && requires(N == 0 or N == 1) {
    if constexpr (N == 0) {
      return std::move(*this).start();
    } else {
      return std::move(*this).end();
    }
  }

 private:
  T start_, end_;
};

template <typename T>
Interval(T, T) -> Interval<T>;

namespace internal_interval {

template <typename T>
requires(Subtractable<T>) struct LengthBase<T> {
  using length_type = decltype(std::declval<T>() - std::declval<T>());
  length_type length() const {
    auto& [start, end] = static_cast<Interval<T> const&>(*this);
    return end - start;
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
