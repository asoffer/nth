#ifndef NTH_META_SEQUENCE_H
#define NTH_META_SEQUENCE_H

#include <concepts>
#include <ostream>

#include "nth/meta/type.h"

namespace nth {
namespace internal_meta {

template <typename T>
concept EqualityComparable = requires(T t) {
  { t == t } -> std::same_as<bool>;
};

template <typename T, T... Vs>
struct Ordered {
  static constexpr bool value = [] {
    constexpr int N    = sizeof...(Vs);
    constexpr T data[] = {Vs...};
    for (int i = 0; i + 1 < N; ++i) {
      if (data[i] > data[i + 1]) return false;
      return true;
    }
  }();
};

template <bool B, typename T, typename F>
auto conditional() {
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
    return (conditional<Predicate(Vs), Sequence<Vs>, Sequence<>> + ...);
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

  static constexpr auto reverse() {
    if constexpr (size() == 0) {
      return Sequence<>{};
    } else {
      using split_type = SplitFirst<Vs...>;
      return Sequence<split_type::head_value>{} +
             typename split_type::template tail<Sequence>{};
    }
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

  template <auto Predicate>
  static constexpr int count() {
    return (static_cast<int>(Predicate(Vs)) + ...);
  }

  template <auto... Rs>
  constexpr Sequence<Vs..., Rs...> operator+(Sequence<Rs...>) const {
    return {};
  }

  template <auto... Rs>
  constexpr bool operator==(Sequence<Rs...>) const
      requires((EqualityComparable<decltype(Rs)> and ...)) {
    if constexpr (sizeof...(Vs) == sizeof...(Rs) and
                  ((type<decltype(Vs)> == type<decltype(Rs)>)and...)) {
      return ((Vs == Rs) and ...);
    } else {
      return false;
    }
  }

  template <auto... Rs>
  constexpr bool operator!=(Sequence<Rs...> m) const
      requires((EqualityComparable<decltype(Rs)> and ...)) {
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

template <typename... Ts>
inline constexpr auto type_sequence = sequence<type<Ts>...>;

template <unsigned N>
static constexpr auto index_sequence =
    internal_meta::IndexSequence<N>::value;

}  // namespace nth

#endif  // NTH_META_SEQUENCE_H
