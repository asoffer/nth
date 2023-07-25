#ifndef NTH_BASE_MACROS_H
#define NTH_BASE_MACROS_H

// NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION
//
// Expands the given expression argument in such a way that it can be used as a
// prefix subexpression, but nowhere else.
#define NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(...)                     \
  switch (0)                                                                   \
  default: __VA_ARGS__

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
  NTH_IS_EMPTY(NTH_SECOND_ARGUMENT(NTH_CONCATENATE(            \
      NTH_INTERNAL_IS_PARENTHESIZED_INJECT_ARGUMENTS_,                         \
      NTH_FIRST_ARGUMENT(NTH_CONCATENATE(                      \
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
  NTH_CONCATENATE(, NTH_IDENTITY p)

#endif  // NTH_BASE_MACROS_H
