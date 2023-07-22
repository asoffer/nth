#ifndef NTH_DEBUG_TRACE_INTERNAL_OPERATORS_H
#define NTH_DEBUG_TRACE_INTERNAL_OPERATORS_H

#include "nth/debug/trace/internal/trace.h"

namespace nth::internal_debug {

#define NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Op, op)                            \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename L, typename R>                                          \
    static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {       \
      return ::nth::internal_debug::Evaluate(lhs)                              \
          op ::nth::internal_debug::Evaluate(rhs);                             \
    }                                                                          \
    template <typename L, typename R>                                          \
    using invoke_type =                                                        \
        decltype(::nth::internal_debug::Evaluate(std::declval<L const &>())    \
                     op ::nth::internal_debug::Evaluate(                       \
                         std::declval<R const &>()));                          \
  };                                                                           \
  template <typename L, typename R>                                            \
  constexpr auto operator op(L const &lhs, R const &rhs) requires(             \
      ::nth::internal_debug::TracedImpl<L> or                                  \
      ::nth::internal_debug::TracedImpl<R>) {                                  \
    return ::nth::internal_debug::Traced<Op, ::nth::internal_debug::Erased<L>, \
                                         ::nth::internal_debug::Erased<R>>(    \
        lhs, rhs);                                                             \
  }

NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Le, <=)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Lt, <)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Eq, ==)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Ne, !=)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Ge, >=)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Gt, >)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Cmp, <=>)

NTH_INTERNAL_DEFINE_BINARY_OPERATOR(And, &)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Or, |)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Xor, ^)

NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Add, +)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Sub, -)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Mul, *)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Div, /)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Mod, %)

NTH_INTERNAL_DEFINE_BINARY_OPERATOR(LSh, <<)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(RSh, >>)
NTH_INTERNAL_DEFINE_BINARY_OPERATOR(ArrowPtr, ->*)

// Analogous to the expansion of the macros above, except that `,` cannot be
// passed as a macro argument, so the body is implemented directly outside the
// macro.
struct Comma {
  static constexpr char name[] = "operator,";

  template <typename L, typename R>
  static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {
    return (::nth::internal_debug::Evaluate(lhs),
            ::nth::internal_debug::Evaluate(rhs));
  }
  template <typename L, typename R>
  using invoke_type =
      decltype((::nth::internal_debug::Evaluate(std::declval<L const &>()),
                ::nth::internal_debug::Evaluate(std::declval<L const &>())));
};
template <typename L, typename R>
constexpr auto operator,(L const &lhs, R const &rhs) requires(
    ::nth::internal_debug::TracedImpl<L> or
    ::nth::internal_debug::TracedImpl<R>) {
  return ::nth::internal_debug::Traced<Comma, ::nth::internal_debug::Erased<L>,
                                       ::nth::internal_debug::Erased<R>>(lhs,
                                                                         rhs);
}

#undef NTH_INTERNAL_DEFINE_BINARY_OPERATOR

#define NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Op, op)                      \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename T>                                                      \
    static constexpr decltype(auto) invoke(T const &t) {                       \
      return op ::nth::internal_debug::Evaluate(t);                            \
    }                                                                          \
    template <typename T>                                                      \
    using invoke_type = decltype(op ::nth::internal_debug::Evaluate(           \
        std::declval<T const &>()));                                           \
  };                                                                           \
  template <::nth::internal_debug::TracedImpl T>                               \
  constexpr auto operator op(T const &t) {                                     \
    return ::nth::internal_debug::Traced<Op,                                   \
                                         ::nth::internal_debug::Erased<T>>(t); \
  }

NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Tilde, ~)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Not, !)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Neg, -)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Pos, +)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Addr, &)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Ref, *)

#undef NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR

}  // namespace nth::internal_debug

#endif  // NTH_DEBUG_TRACE_INTERNAL_OPERATORS_H
