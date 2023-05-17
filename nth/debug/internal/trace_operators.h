#ifndef NTH_DEBUG_INTERNAL_TRACE_OPERATORS_H
#define NTH_DEBUG_INTERNAL_TRACE_OPERATORS_H

#include "nth/debug/internal/trace.h"

namespace nth::internal_trace {

#define NTH_INTERNAL_DEFINE_BINARY_OPERATOR(Op, op)                            \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename L, typename R>                                          \
    static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {       \
      return ::nth::internal_trace::Evaluate(lhs)                              \
          op ::nth::internal_trace::Evaluate(rhs);                             \
    }                                                                          \
    template <typename L, typename R>                                          \
    using invoke_type =                                                        \
        decltype(::nth::internal_trace::Evaluate(std::declval<L const &>())    \
                     op ::nth::internal_trace::Evaluate(                       \
                         std::declval<R const &>()));                          \
  };                                                                           \
  template <typename L, typename R>                                            \
  auto operator op(L const &lhs, R const &rhs) requires(                       \
      ::nth::internal_trace::IsTraced<L> or                                    \
      ::nth::internal_trace::IsTraced<R>) {                                    \
    return Traced<Op, L, R>(lhs, rhs);                                         \
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
// passed as a macro argument, so the body is implemented directly outside the macro.
struct Comma {
  template <typename L, typename R>
  static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {
    return (::nth::internal_trace::Evaluate(lhs),
            ::nth::internal_trace::Evaluate(rhs));
  }
  template <typename L, typename R>
  using invoke_type =
      decltype((::nth::internal_trace::Evaluate(std::declval<L const &>()),
                ::nth::internal_trace::Evaluate(std::declval<L const &>())));
};
template <typename L, typename R>
auto operator,(L const &lhs,
               R const &rhs) requires(::nth::internal_trace::IsTraced<L> or
                                      ::nth::internal_trace::IsTraced<R>) {
  return Traced<Comma, L, R>(lhs, rhs);
}

#undef NTH_INTERNAL_DEFINE_BINARY_OPERATOR

#define NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Op, op)                      \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename T>                                                      \
    static constexpr decltype(auto) invoke(T const &t) {                       \
      return op ::nth::internal_trace::Evaluate(t);                            \
    }                                                                          \
    template <typename T>                                                      \
    using invoke_type = decltype(op ::nth::internal_trace::Evaluate(           \
        std::declval<T const &>()));                                           \
  };                                                                           \
  template <::nth::internal_trace::IsTraced T>                                 \
  auto operator op(T const &t) {                                               \
    return Traced<Op, T>(t);                                                   \
  }

NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Tilde, ~)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Not, !)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Neg, -)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Pos, +)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Addr, &)
NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Ref, *)

#undef NTH_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR

}  // namespace nth::internal_trace

#endif  // NTH_DEBUG_INTERNAL_TRACE_OPERATORS_H
