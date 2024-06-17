#ifndef NTH_META_SEQUENCE_H
#define NTH_META_SEQUENCE_H

#include <array>
#include <concepts>
#include <ostream>
#include <type_traits>
#include <utility>

#include "nth/meta/concepts/comparable.h"

namespace nth {
namespace internal_meta {

template <auto V>
struct TypeWrap {
  static constexpr auto value = V;
};

template <bool B, typename T, typename F>
constexpr auto conditional() {
  if constexpr (B) {
    return T{};
  } else {
    return F{};
  }
};

template <auto V, auto... Vs>
struct SplitFirst {
  static constexpr auto head_value = V;

  template <template <auto...> typename F>
  using tail = F<Vs...>;
};

template <auto...>
struct Sequence;

template <auto... Vs>
std::true_type IsSequenceImpl(Sequence<Vs...> const*);
std::false_type IsSequenceImpl(...);

template <unsigned>
struct IndexSequence;

template <auto... Vs>
struct Sequence {
  static constexpr int size() { return sizeof...(Vs); }
  static constexpr bool empty() { return size() == 0; }

  template <auto F>
  static constexpr Sequence<F(Vs)...> transform() {
    return {};
  }

  static constexpr auto reduce(std::invocable<decltype(Vs)...> auto F) {
    return F(Vs...);
  }

  template <auto Predicate>
  static constexpr auto filter() {
    return (conditional<Predicate(Vs), Sequence<Vs>, Sequence<>>() + ... +
            Sequence<>());
  }

  template <size_t N>
  static constexpr auto chunk() requires(size() % N == 0) {
    if constexpr (empty()) {
      return Sequence<>{};
    } else {
      return []<size_t... Ns>(std::integer_sequence<size_t, Ns...>) {
        return Sequence<select<Ns...>()>{} + drop<N>().template chunk<N>();
      }
      (std::make_index_sequence<N>{});
    }
  }

  static constexpr auto unique() {
    if constexpr (empty()) {
      return Sequence<>{};
    } else {
      constexpr auto t = tail().unique();
      constexpr auto h = head();
      if constexpr (t.template contains<h>()) {
        return t;
      } else {
        return Sequence<h>{} + t;
      }
    }
  }

  static constexpr auto head() requires(sizeof...(Vs) > 0) {
    return SplitFirst<Vs...>::head_value;
  }

  static constexpr auto tail() requires(sizeof...(Vs) > 0) {
    using split_type = SplitFirst<Vs...>;
    return typename split_type::template tail<Sequence>{};
  }

  template <size_t N = 1>
  [[nodiscard]] static constexpr auto drop() requires(sizeof...(Vs) >= N) {
    if constexpr (N == 1) {
      using split_type = SplitFirst<Vs...>;
      return typename split_type::template tail<Sequence>{};
    } else {
      return []<size_t... Ns>(std::integer_sequence<size_t, Ns...>) {
        return select<(N + Ns)...>();
      }
      (std::make_index_sequence<sizeof...(Vs) - N>{});
    }
  }

  template <size_t N = 1>
  [[nodiscard]] static constexpr auto drop_back() requires(sizeof...(Vs) >= N) {
    return []<size_t... Ns>(std::integer_sequence<size_t, Ns...>) {
      return select<Ns...>();
    }
    (std::make_index_sequence<sizeof...(Vs) - N>{});
  }

  template <size_t... Ns>
  static constexpr auto select() {
    return Sequence<__type_pack_element<Ns, TypeWrap<Vs>...>::value...>{};
  }

  template <size_t N>
  static constexpr auto get() requires(N <= sizeof...(Vs)) {
    return select<N>().head();
  }

  static constexpr auto reverse() {
    return []<size_t... Ns>(std::integer_sequence<size_t, Ns...>) {
      return select<(sizeof...(Vs) - 1 - Ns)...>();
    }
    (std::make_index_sequence<sizeof...(Vs)>{});
  }

  static constexpr auto flatten() requires(
      (decltype(IsSequenceImpl(
           static_cast<decltype(Vs) const*>(nullptr)))::value and
       ...)) {
    return (Vs + ...);
  }

  template <auto Predicate>
  static constexpr bool all() {
    return (Predicate(Vs) and ...);
  }

  template <auto Predicate>
  static constexpr bool any() {
    return (Predicate(Vs) or ...);
  }

  static constexpr auto to_array() { return std::array{Vs...}; }

  template <auto Predicate>
  static constexpr int count() {
    return (static_cast<int>(Predicate(Vs)) + ...);
  }

  static constexpr void each(auto f) { (f(Vs), ...); }

  template <auto... Rs>
  constexpr Sequence<Vs..., Rs...> operator+(Sequence<Rs...>) const {
    return {};
  }

  template <auto... Rs>
  constexpr bool operator==(Sequence<Rs...>) const
      requires((nth::equality_comparable<decltype(Rs)> and ...)) {
    if constexpr (sizeof...(Vs) == sizeof...(Rs) and
                  ((std::is_same_v<decltype(Vs), decltype(Rs)>)and...)) {
      return ((Vs == Rs) and ...);
    } else {
      return false;
    }
  }

  template <auto... Rs>
  constexpr bool operator!=(Sequence<Rs...> m) const
      requires((nth::equality_comparable<decltype(Rs)> and ...)) {
    return not operator==(m);
  }

  template <auto V>
  static constexpr bool contains() {
    return ((V == Vs) or ...);
  }

  template <auto Predicate>
  static constexpr int find_if() {
    int result = -1;
    ((++result, Predicate(Vs)) or ...);
    return result;
  }

  friend std::ostream& operator<<(std::ostream& os, Sequence) {
    char const* separator = "[";
    (((os << separator << Vs), separator = ", "), ...);
    return os << "]";
  }
};

template <unsigned N>
struct IndexSequence {
  static constexpr auto value =
      (IndexSequence<N - 1>::value + Sequence<static_cast<unsigned>(N - 1)>{});
};

template <>
struct IndexSequence<0u> {
  static constexpr Sequence<> value = {};
};

}  // namespace internal_meta

template <typename T>
concept Sequence = decltype(internal_meta::IsSequenceImpl(
    static_cast<T const*>(nullptr)))::value;

template <auto... Vs>
inline constexpr internal_meta::Sequence<Vs...> sequence;

template <unsigned N>
static constexpr auto index_sequence = internal_meta::IndexSequence<N>::value;

}  // namespace nth

#endif  // NTH_META_SEQUENCE_H
