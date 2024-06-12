#ifndef NTH_BASE_MACROS_H
#define NTH_BASE_MACROS_H

#include "nth/base/platform.h"

// NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION
//
// Expands the given expression argument in such a way that it can be used as a
// prefix subexpression, but nowhere else.
#define NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(...)                     \
  switch (0)                                                                   \
  default: __VA_ARGS__

// NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE
//
// Expands to a statement that is invalid except within the global namespace.
#define NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                              \
  struct NthRequireExpansionInGlobalNamespace;                                 \
  static_assert(sizeof(NthRequireExpansionInGlobalNamespace));

struct NthRequireExpansionInGlobalNamespace {};

// NTH_STRINGIFY
//
// Expands to the first argument passed to the macro.
#define NTH_STRINGIFY(...) NTH_INTERNAL_STRINGIFY(__VA_ARGS__)
#define NTH_INTERNAL_STRINGIFY(...) #__VA_ARGS__

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

// NTH_TAIL_ARGUMENTS
//
// Expands to all arguments except the first.
#define NTH_TAIL_ARGUMENTS(...) NTH_INTERNAL_TAIL_ARGUMENTS(__VA_ARGS__)
#define NTH_INTERNAL_TAIL_ARGUMENTS(a, ...) __VA_ARGS__

// NTH_CONCATENATE
//
// Expands to its arguments except that the last token of the first argument and
// the first token of the second argument are concatenated together.
#define NTH_CONCATENATE(head, ...) NTH_INTERNAL_CONCATENATE(head, __VA_ARGS__)
#define NTH_INTERNAL_CONCATENATE(head, ...) head##__VA_ARGS__

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
// The argument bound to `condition` must expand to either `true` or `false`. If
// it expands to `true`, then the entire macro expands to the argument bound to
// `t`. Otherwise the entire macro expands to the argument bound to `f`.
#define NTH_IF(condition, t, f) NTH_INTERNAL_IF(condition, t, f)
#define NTH_INTERNAL_IF(condition, t, f) NTH_INTERNAL_BRANCH_##condition(t, f)
#define NTH_INTERNAL_BRANCH_false(t, f) f
#define NTH_INTERNAL_BRANCH_true(t, f) t

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
  NTH_IS_EMPTY(NTH_SECOND_ARGUMENT(                                            \
      NTH_CONCATENATE(NTH_INTERNAL_IS_PARENTHESIZED_INJECT_ARGUMENTS_,         \
                      NTH_FIRST_ARGUMENT(NTH_CONCATENATE(                      \
                          NTH_INTERNAL_IS_PARENTHESIZED_PREFIXED_,             \
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
         NTH_INTERNAL_IGNORE_PARENTHESES_REMOVE, NTH_IDENTITY)                 \
  (argument)

#define NTH_INTERNAL_IGNORE_PARENTHESES_REMOVE(p)                              \
  NTH_CONCATENATE(, NTH_IDENTITY p)

// `NTH_INLINE_CODE_SNIPPET_BEGIN`, `NTH_INLINE_CODE_SNIPPET_END`, and
// are used to define self-contained blocks of code to be used inline, typically
// expanded from other macros. While one could routinely use a lambda for this,
// compilation of lambdas can be expensive on some compilers, requiring
// instantiation of new types. In many cases, on compilers that support them,
// GNU-style statement expressions are a more efficient tool for this job. These
// macros expand to statement expressions when they would be available and to
// lambdas otherwise.
//
// An inline code block begins with `NTH_INLINE_CODE_SNIPPET_BEGIN` and ends
// with an invocation of the function-like macro `NTH_INLINE_CODE_SNIPPET_END`,
// where the arguments provided to the macro are the expression to which the
// code snippet should evaluate.
//
// As an example, one might define:
// #define STATIC_COUNTER                                                      \
//   NTH_INLINE_CODE_SNIPPET_BEGIN                                             \
//   static int counter = 0;                                                   \
//   NTH_INLINE_CODE_SNIPPET_END(counter++)
//
// Each time the line `int x = STATIC_COUNTER;` is reached, `x` will take on the
// next value integer value.
//
// Note that the expression passed to `NTH_INLINE_CODE_SNIPPET_END` must
// evaluate unconditionally (i.e., it must not contain any
// statement-expression-like control flow.
//
#if NTH_COMPILER(clang) or NTH_COMPILER(gcc)

#define NTH_INLINE_CODE_SNIPPET_BEGIN ({
#define NTH_INTERNAL_INLINE_CODE_SNIPPET_END_NONEMPTY(...)                     \
  __VA_ARGS__;                                                                 \
  })
#define NTH_INTERNAL_INLINE_CODE_SNIPPET_END_EMPTY(...)                        \
  (void)0;                                                                     \
  })

#define NTH_INLINE_CODE_SNIPPET_END(...)                                       \
  NTH_IF(NTH_IS_EMPTY(__VA_ARGS__),                                            \
         NTH_INTERNAL_INLINE_CODE_SNIPPET_END_EMPTY,                           \
         NTH_INTERNAL_INLINE_CODE_SNIPPET_END_NONEMPTY)                        \
  (__VA_ARGS__)

#else

#define NTH_INLINE_CODE_SNIPPET_BEGIN ([&] {
#define NTH_INLINE_CODE_SNIPPET_END(...)                                       \
  return __VA_ARGS__;                                                          \
  }())

#endif

#endif  // NTH_BASE_MACROS_H
