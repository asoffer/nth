#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACE_H

#include <type_traits>
#include <utility>

#include "nth/base/macros.h"
#include "nth/debug/trace/internal/actions.h"
#include "nth/debug/trace/internal/api.h"
#include "nth/debug/trace/internal/implementation.h"
#include "nth/meta/type.h"

namespace nth::debug::internal_trace {

#define NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Op, op)               \
  template <typename L, typename R>                                            \
  constexpr auto operator op(L const &lhs, R const &rhs) requires(             \
      internal_trace::TracedImpl<L> or internal_trace::TracedImpl<R>) {        \
    return internal_trace::TracedExpr<Op, internal_trace::Erased<L>,           \
                                      internal_trace::Erased<R>>(lhs, rhs);    \
  }

NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Le, <=)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Lt, <)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Eq, ==)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Ne, !=)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Ge, >=)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Gt, >)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Cmp, <=>)

NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(And, &)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Or, |)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Xor, ^)

NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Add, +)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Sub, -)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Mul, *)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Div, /)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Mod, %)

NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(LSh, <<)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(RSh, >>)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(ArrowPtr, ->*)

// Analogous to the expansion of the macros above, except that `,` cannot be
// easily passed as a macro argument, so the body is implemented directly
// outside the macro.
template <typename L, typename R>
constexpr auto operator,(L const &lhs,
                         R const &rhs) requires(internal_trace::TracedImpl<L> or
                                                internal_trace::TracedImpl<R>) {
  return internal_trace::TracedExpr<Comma, internal_trace::Erased<L>,
                                    internal_trace::Erased<R>>(lhs, rhs);
}
#undef NTH_DEBUG_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR

#define NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Op, op)                \
  template <internal_trace::TracedImpl T>                                      \
  constexpr auto operator op(T const &t) {                                     \
    return internal_trace::TracedExpr<Op, internal_trace::Erased<T>>(t);       \
  }

NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Tilde, ~)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Not, !)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Neg, -)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Pos, +)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Addr, &)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Ref, *)

#undef NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACE_H
