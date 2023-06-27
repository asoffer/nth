#ifndef NTH_BASE_MACROS_H
#define NTH_BASE_MACROS_H

#include "nth/base/internal/macros.h"

// This file defines commonly used macros and utilities for interacting with
// macros. All macros are prefixed with `NTH_`. Macros that are prefixed with
// `NTH_INTERNAL_` are implementation details not to be explicitly invoked by
// end users. All other macros are part of the public API.

// NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE
//
// Expands to a declaration which will compile if and only if it is in the
// global namespace.
#define NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                              \
  namespace nth::internal_macros {                                             \
  static_assert(macro_must_be_expanded_in_the_global_namespace);               \
  }                                                                            \
  static_assert(true, "require a semicolon")

// NTH_TYPE
//
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

// NTH_TYPE_COUNT
//
// Expands to the number of types listed in the macro. For example, despite the
// fact that `NTH_TYPE_COUNT(std::pair<int, bool>, char)` has three macro
// arguments, the expanded expression will be a constant expression evaluating
// to 2.
#define NTH_TYPE_COUNT(...)                                                    \
  ::nth::internal_macros::type_count<void(                                     \
      nth::internal_macros::type_list<__VA_ARGS__>)>::count

// NTH_FIRST_ARGUMENT
//
// Expands to the first argument passed to the macro.
#define NTH_FIRST_ARGUMENT(...) NTH_INTERNAL_FIRST_ARGUMENT(__VA_ARGS__)
#define NTH_INTERNAL_FIRST_ARGUMENT(p, ...) p

// NTH_SECOND_ARGUMENT
//
// Expands to the first argument passed to the macro.
#define NTH_SECOND_ARGUMENT(...) NTH_INTERNAL_SECOND_ARGUMENT(__VA_ARGS__)
#define NTH_INTERNAL_SECOND_ARGUMENT(a, b, ...) b

// NTH_INVOKE
//
// Invokes the first argument passed to the macro on the remaining arguments.
#define NTH_INVOKE(...) NTH_INTERNAL_INVOKE_IMPL(__VA_ARGS__)
#define NTH_INTERNAL_INVOKE_IMPL(f, ...) f(__VA_ARGS__)

// NTH_IDENTITY
//
// Expands to the arguments passed in.
#define NTH_IDENTITY(...) __VA_ARGS__

// NTH_NOT
//
// The macro argument must expand to either `true` or `false`. The entire macro
// will expand to `true` if the argument expands to `false`, and `false` if the
// argument expands to `true`.
#define NTH_NOT(b) NTH_INTERNAL_NOT(b)
#define NTH_INTERNAL_NOT(b) NTH_INTERNAL_NOT_##b
#define NTH_INTERNAL_NOT_true false
#define NTH_INTERNAL_NOT_false true

// NTH_IF
//
// Invokes either `t` or `f` on `__VA_ARGS__`. `condition` must expand to either
// `true` or `false`. If `condition` expands to `true`, the macro expands to the
// macro `t` invoked on `__VA_ARGS__`. Otherwise (if `condition` expands to
// `false`), the macro expands to the macro `f` invoked on `__VA_ARGS__`.
#define NTH_IF(condition, t, f, ...)                                           \
  NTH_INTERNAL_IF(condition, t, f, __VA_ARGS__)
#define NTH_INTERNAL_IF(condition, t, f, ...)                                  \
  NTH_INTERNAL_BRANCH_##condition(t, f, __VA_ARGS__)
#define NTH_INTERNAL_BRANCH_false(t, f, ...) f(__VA_ARGS__)
#define NTH_INTERNAL_BRANCH_true(t, f, ...) t(__VA_ARGS__)

// NTH_IS_EMPTY
//
// Expands to `true` if no variadic arguments are passed and to `false`
// otherwise.
#define NTH_IS_EMPTY(...) NTH_INTERNAL_IS_EMPTY_##__VA_OPT__(false)
#define NTH_INTERNAL_IS_EMPTY_ true
#define NTH_INTERNAL_IS_EMPTY_false false

// NTH_IS_PARENTHESIZED
//
// Expands to `true` if the argument is entirely enclosed in parentheses, and to
// `false` otherwise.
#define NTH_IS_PARENTHESIZED(x)                                                \
  NTH_IS_EMPTY(NTH_SECOND_ARGUMENT(NTH_INTERNAL_EXPAND_WITH_PREFIX(            \
      NTH_INTERNAL_IS_PARENTHESIZED_INJECT_ARGUMENTS_,                         \
      NTH_FIRST_ARGUMENT(NTH_INTERNAL_EXPAND_WITH_PREFIX(                      \
          NTH_INTERNAL_IS_PARENTHESIZED_PREFIXED_,                             \
          NTH_INTERNAL_IS_PARENTHESIZED_REMOVE x)))))
#define NTH_INTERNAL_IS_PARENTHESIZED_INJECT_ARGUMENTS_true _,
#define NTH_INTERNAL_IS_PARENTHESIZED_INJECT_ARGUMENTS_false _, _

#define NTH_INTERNAL_IS_PARENTHESIZED_REMOVE(...)                              \
  NTH_INTERNAL_IS_PARENTHESIZED_TRUE
#define NTH_INTERNAL_IS_PARENTHESIZED_PREFIXED_NTH_INTERNAL_IS_PARENTHESIZED_TRUE \
  true
#define NTH_INTERNAL_IS_PARENTHESIZED_PREFIXED_NTH_INTERNAL_IS_PARENTHESIZED_REMOVE \
  false,

// NTH_IGNORE_PARENTHESES
//
// Expands either to its argument, if it is not parenthesesized, or to it
// arguments with the outermost parentheses removed, if it is surrounded by
// parentheses.
#define NTH_IGNORE_PARENTHESES(argument)                                       \
  NTH_IF(NTH_IS_PARENTHESIZED(argument),                                       \
         NTH_INTERNAL_IGNORE_PARENTHESES_REMOVE, NTH_IDENTITY, argument)

#define NTH_INTERNAL_IGNORE_PARENTHESES_REMOVE(p)                              \
  NTH_INTERNAL_EXPAND_WITH_PREFIX(, NTH_IDENTITY p)

#endif  // NTH_BASE_MACROS_H
