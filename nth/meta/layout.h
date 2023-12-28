#ifndef NTH_META_LAYOUT_H
#define NTH_META_LAYOUT_H

#include <cstddef>
#include <climits>

#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace nth::layout {

inline constexpr size_t bits_per_byte = CHAR_BIT;

namespace internal_layout {

struct UsedCategory;
struct UnusedCategory;
struct SetCategory;
struct UnsetCategory;

template <size_t N>
struct Used {
  using category = UsedCategory;
  template <size_t R>
  friend constexpr auto operator+(Used, Used<R>) {
    return Used<N + R>{};
  }

  static constexpr size_t size() { return N; }
};

template <size_t N>
struct Unused {
  using category = UnusedCategory;
  template <size_t R>
  friend constexpr auto operator+(Unused, Unused<R>) {
    return Unused<N + R>{};
  }

  static constexpr size_t size() { return N; }
};

template <size_t N>
struct Set {
  using category = SetCategory;
  template <size_t R>
  friend constexpr auto operator+(Set, Set<R>) {
    return Set<N + R>{};
  }

  static constexpr size_t size() { return N; }
};

template <size_t N>
struct Unset {
  using category = UnsetCategory;
  template <size_t R>
  friend constexpr auto operator+(Unset, Unset<R>) {
    return Unset<N + R>{};
  }

  static constexpr size_t size() { return N; }
};

template <typename... Chunks>
struct LayoutBitSequence {
  template <typename RChunk, typename... RChunks>
  friend constexpr auto operator+(LayoutBitSequence,
                                  LayoutBitSequence<RChunk, RChunks...>) {
    constexpr auto l                = type_sequence<Chunks...>;
    constexpr auto l_last           = l.template get<l.size() - 1>();
    constexpr auto l_last_category  = type<typename type_t<l_last>::category>;
    constexpr auto r_first_category = type<typename RChunk::category>;
    if constexpr (l_last_category == r_first_category) {
      return l.drop_back().reduce([](auto... ts) {
        return LayoutBitSequence<
            type_t<ts>..., decltype(type_t<l_last>{} + RChunk{}), RChunks...>{};
      });
    } else {
      return LayoutBitSequence<Chunks..., RChunk, RChunks...>{};
    }
  }
  friend constexpr auto operator+(LayoutBitSequence l, LayoutBitSequence<>) {
    return l;
  }

  static constexpr size_t size() { return (Chunks::size() + ...); };
};

template <typename... Ls, typename... Rs>
inline consteval bool operator==(LayoutBitSequence<Ls...>,
                                 LayoutBitSequence<Rs...>) {
  return type_sequence<Ls...> == type_sequence<Rs...>;
}

template <typename... Ls, typename... Rs>
inline consteval bool operator!=(LayoutBitSequence<Ls...> lhs,
                                 LayoutBitSequence<Rs...> rhs) {
  return not(lhs == rhs);
}

template <size_t Alignment, typename BitSequence>
struct Layout {
  static constexpr size_t alignment() { return Alignment; }
  static constexpr auto bit_sequence() { return BitSequence{}; }
};

template <size_t LhsAlignment, typename LhsBitSequence, size_t RhsAlignment,
          typename RhsBitSequence>
inline consteval bool operator==(Layout<LhsAlignment, LhsBitSequence>,
                                 Layout<RhsAlignment, RhsBitSequence>) {
  return LhsAlignment == RhsAlignment and LhsBitSequence{} == RhsBitSequence{};
}
template <size_t LhsAlignment, typename LhsBitSequence, size_t RhsAlignment,
          typename RhsBitSequence>
inline consteval bool operator!=(Layout<LhsAlignment, LhsBitSequence> lhs,
                                 Layout<RhsAlignment, RhsBitSequence> rhs) {
  return not(lhs == rhs);
}

template <typename T>
struct LayoutFor {
  // By default, we assume all bits are used.
  using type =
      Layout<alignof(T), LayoutBitSequence<Used<sizeof(T) * bits_per_byte>>>;
};

template <typename T>
struct LayoutFor<T const> : LayoutFor<T> {};
template <typename T>
struct LayoutFor<T volatile> : LayoutFor<T> {};
template <typename T>
struct LayoutFor<T const volatile> : LayoutFor<T> {};

inline constexpr size_t Log2(size_t N) {
  size_t i = 0;
  while (N > 1) {
    N /= 2;
    ++i;
  }
  return i;
}

template <typename T>
struct LayoutFor<T*> {
  using type = Layout<
      alignof(T),
      LayoutBitSequence<Used<sizeof(void*) * bits_per_byte - Log2(alignof(T))>,
                        Unset<Log2(alignof(T))>>>;
};

}  // namespace internal_layout

template <size_t N>
inline constexpr internal_layout::LayoutBitSequence<internal_layout::Used<N>>
    used;
template <size_t N>
inline constexpr internal_layout::LayoutBitSequence<internal_layout::Unused<N>>
    unused;
template <size_t N>
inline constexpr internal_layout::LayoutBitSequence<internal_layout::Set<N>>
    set;
template <size_t N>
inline constexpr internal_layout::LayoutBitSequence<internal_layout::Unset<N>>
    unset;

template <typename T>
inline constexpr auto of = typename internal_layout::LayoutFor<T>::type{};

}  // namespace nth::layout

#endif  // NTH_META_LAYOUT_H
