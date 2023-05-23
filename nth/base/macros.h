#ifndef NTH_BASE_MACROS_H
#define NTH_BASE_MACROS_H

#include <type_traits>

// This file defines commonly used macros and utilities for interacting with
// macros.

namespace nth {
namespace internal_macros {

inline constexpr bool macro_must_be_expanded_in_the_global_namespace = true;

template <typename T>
struct qualifiers {
  static constexpr int value = 0;
};
template <typename T>
struct qualifiers<T&> {
  static constexpr int value = 1;
};
template <typename T>
struct qualifiers<T&&> {
  static constexpr int value = 2;
};

template <typename T, int Q>
struct explicitly_qualified_type {
  using type = T;
};
template <typename T>
struct explicitly_qualified_type<T, 1> {
  using type = T&;
};
template <typename T>
struct explicitly_qualified_type<T, 2> {
  using type = T&&;
};

// Types used below need to listed as pointers to support both incomplete types
// and pure-virtual types. The use of `explicitly_qualified_types` also enables
// us to support types with qualifiers.
template <typename... Ts>
using type_list = void (*)(nth::internal_macros::explicitly_qualified_type<
                           std::remove_reference_t<Ts>,
                           nth::internal_macros::qualifiers<Ts>::value>*...);

// In the templates below the `type_list` is wrapped in an extra `void(...)` so
// that the macros defined below can use `void(...)` as a mechanism to make sure
// the macro cannot be unintentionally exploited. If the type list provided in
// the macro was not expanded into an expression using parentheses, but just one
// with angle-brackets, a user could attempt to close an angle-bracket inside
// the macro and inject other code.

template <typename>
struct type_count;

template <typename... Ts>
struct type_count<void(void (*)(Ts...))> {
  static constexpr int count = sizeof...(Ts);
};

template <int, typename>
struct get_type;

template <int N, typename... Ts>
struct get_type<N, void(void (*)(Ts...))> : type_count<void(void (*)(Ts...))> {
  static_assert(sizeof...(Ts) > N,
                "Too few types expanded from macro argument.");

 private:
  using explicitly_qualified_type =
      std::remove_pointer_t<__type_pack_element<N, Ts...>>;

 public:
  using type = typename explicitly_qualified_type::type;
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
// useful, when viable, to accept macro arguments via `...` and expand them via
// `__VA_ARGS__` so that regardless of how they are used, the appropriate types
// will be preserved. However this makes it difficult to access the type at any
// particular index, which is what `NTH_TYPE` provides.
#define NTH_TYPE(index, ...)                                                   \
  typename ::nth::internal_macros::get_type<                                   \
      index, void(nth::internal_macros::type_list<__VA_ARGS__>)>::type

// Expands to the number of types listed in the macro. For example, despite the
// fact that `NTH_TYPE_COUNT(std::pair<int, bool>, char)` has three macro
// arguments, the expanded expression will be a constant expression evaluating
// to 2.
#define NTH_TYPE_COUNT(...)                                                    \
  ::nth::internal_macros::type_count<void(                                     \
      nth::internal_macros::type_list<__VA_ARGS__>)>::count

}  // namespace nth

#endif  // NTH_BASE_MACROS_H
