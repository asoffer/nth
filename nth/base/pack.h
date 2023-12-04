#ifndef NTH_BASE_PACK_H
#define NTH_BASE_PACK_H

#include <cstddef>

#include "nth/base/macros.h"

// Defines a collection of macros that wrap common parameter-pack expansion
// idioms.
//
// NTH_PACK:
//
// The NTH_PACK macro takes a first parenthesized argument which dictates the
// action it takes on the remaining arguments. The first parenthesized argument
// may be a single identifier such as `any` or `last`, or a may be such an
// identifier followed by further arguments whose use depends on the identifier
// (see full list below).
//
// NTH_PACK((each), <pack>):
// Evaluates each expression in the pack. Evaluation is guaranteed to happen in
// pack-order.
//
// NTH_PACK((each_reverse), <pack>):
// Evaluates each expression in the pack. Evaluation is guaranteed to happen in
// reverse-pack-order.
//
// NTH_PACK((invoke, <fn>), <pack>):
// Applies `fn` to each member of `pack`. Function invocation is guaranteed to
// happen in pack-order.
//
// NTH_PACK((any, <predicate>), <pack>):
// Evaluates to `true` if any member of `pack` satisfies `predicate`, and false
// otherwise.
//
// NTH_PACK((all, <predicate>), <pack>):
// Evaluates to `true` if every member of `pack` satisfies `predicate`, and
// false otherwise.
//
// NTH_PACK((count_if, <predicate>), <pack>):
// Evaluates to a `size_t` equal to the number of members of `pack` that satisfy
// `predicate`.
//
#define NTH_PACK(action, ...)                           \
    NTH_PACK_INTERNAL_ACTION(NTH_FIRST_ARGUMENT action) \
    ((NTH_TAIL_ARGUMENTS action), __VA_ARGS__)
#define NTH_PACK_INTERNAL_ACTION(...) NTH_PACK_INTERNAL_ACTION_IMPL(__VA_ARGS__)
#define NTH_PACK_INTERNAL_ACTION_IMPL(action) NTH_PACK_INTERNAL_##action

#define NTH_PACK_INTERNAL_each(unused, ...)                                    \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(((__VA_ARGS__), ...))

#define NTH_PACK_INTERNAL_each_reverse(unused, ...)                            \
  NTH_PACK_INTERNAL_each_reverse_IMPL(__LINE__, __VA_ARGS__)
#define NTH_PACK_INTERNAL_each_reverse_IMPL(line, ...)                         \
  NTH_PACK_INTERNAL_each_reverse_IMPL2(line, __VA_ARGS__)
#define NTH_PACK_INTERNAL_each_reverse_IMPL2(line, ...)                         \
  [[maybe_unused]] int NthInternalUnusedDummyVariableOnLine##line;             \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      (void)(NthInternalUnusedDummyVariableOnLine##line = ... =                \
                 ((void)(__VA_ARGS__), 0)))

#define NTH_PACK_INTERNAL_invoke(fn, ...)                                      \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      ((void)(NTH_IGNORE_PARENTHESES(fn)(__VA_ARGS__)), ...))

#define NTH_PACK_INTERNAL_any(p, ...)                                          \
  (NTH_IGNORE_PARENTHESES(p)(__VA_ARGS__) or ...)

#define NTH_PACK_INTERNAL_all(p, ...)                                          \
  (NTH_IGNORE_PARENTHESES(p)(__VA_ARGS__) and ...)

#define NTH_PACK_INTERNAL_count_if(p, ...)                                     \
  (::std::size_t{} + ... + (NTH_IGNORE_PARENTHESES(p)(__VA_ARGS__) ? 1 : 0))

#endif  // NTH_BASE_PACK_H
