#ifndef NTH_BASE_MACROS_H
#define NTH_BASE_MACROS_H

// This file defines commonly used macros and utilities for interacting with
// macros.

namespace nth {
namespace internal_macros {

inline constexpr bool macro_must_be_expanded_in_the_global_namespace = true;

template < typename... Ts>
struct type_count {
  static constexpr int count = sizeof...(Ts);
};

template <int N, typename... Ts>
struct get_type : type_count<Ts...> {
  static_assert(sizeof...(Ts) > N,
                "Too few types expanded from macro argument.");
  using type = __type_pack_element<N, Ts...>;
};

}  // namespace internal_macros

// Expands to a declaration which will compile if and only if it is in the
// global namespace.
#define NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                              \
  namespace nth::internal_macros {                                             \
  static_assert(macro_must_be_expanded_in_the_global_namespace);               \
  }                                                                            \
  static_assert(true, "require a semicolon")

// Given an integer index and a pack of types, expands to an expression
// evaluating to the type in the pack at the given index.
//
// Macros often do not interact well with types generated from templates. Macro
// expansion does not understand `<` and `>` as a bracketing mechanism. This
// means that a macro will treat `std::pair<A, B>` as two arguments (the first
// being `std::pair<A`, and the second being `B>`). To avoid this problem it is
// useful to accept arguments via `...` and expand them via `__VA_ARGS__`.
// However this does provide an easy mechanism for naming the type at any
// particular index, which is what `NTH_TYPE` provides.
#define NTH_TYPE(index, ...)                                                   \
  typename ::nth::internal_macros::get_type<index, __VA_ARGS__>::type

// Expands to the number of types listed in the macro. For example, despite the
// fact that `NTH_TYPE_COUNT(std::pair<int, bool>, char)` has three macro
// arguments, the expanded expression will be a constant expression evaluating
// to 2.
#define NTH_TYPE_COUNT(...)                                                    \
  ::nth::internal_macros::type_count<__VA_ARGS__>::count

}  // namespace nth

#endif  // NTH_BASE_MACROS_H
