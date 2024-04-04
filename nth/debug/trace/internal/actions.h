#ifndef NTH_DEBUG_TRACE_INTERNAL_ACTIONS_H
#define NTH_DEBUG_TRACE_INTERNAL_ACTIONS_H

#include <type_traits>

#include "nth/debug/trace/internal/traced_value.h"
#include "nth/meta/compile_time_string.h"

namespace nth::debug::internal_trace {

template <compile_time_string S>
struct IdentityAction {
  using NthInternalDebugIdentityAction = void;

  static constexpr auto name = S;

  template <typename T>
  using invoke_type = T;

  template <typename T>
  static constexpr decltype(auto) invoke(T const &t) {
    return t;
  }
};

#define NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Op, op)               \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename L, typename R>                                          \
    static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {       \
      return internal_trace::Evaluate(lhs) op internal_trace::Evaluate(rhs);   \
    }                                                                          \
    template <typename L, typename R>                                          \
    using invoke_type =                                                        \
        decltype(internal_trace::Evaluate(std::declval<L const &>())           \
                     op internal_trace::Evaluate(std::declval<R const &>()));  \
  };

NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Le, <=)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Lt, <)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Eq, ==)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Ne, !=)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Ge, >=)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Gt, >)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Cmp, <=>)

NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(And, &)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Or, |)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Xor, ^)

NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Add, +)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Sub, -)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Mul, *)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Div, /)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(Mod, %)

NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(LSh, <<)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(RSh, >>)
NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION(ArrowPtr, ->*)

#undef NTH_DEBUG_INTERNAL_DEFINE_BINARY_OPERATOR_ACTION

#define NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Op, op)                \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" #op;                             \
    template <typename T>                                                      \
    static constexpr decltype(auto) invoke(T const &t) {                       \
      return op internal_trace::Evaluate(t);                                   \
    }                                                                          \
    template <typename T>                                                      \
    using invoke_type =                                                        \
        decltype(op internal_trace::Evaluate(std::declval<T const &>()));      \
  };

NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Tilde, ~)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Not, !)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Neg, -)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Pos, +)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Addr, &)
NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Ref, *)

#undef NTH_DEBUG_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR

// Similar to the above operator actions, but these operators must be
// implemented as member functions.
struct Index {
  static constexpr char name[] = "operator[]";
  template <typename T, typename... Indices>
  static constexpr decltype(auto) invoke(T const &t,
                                         Indices const &...indices) {
    return internal_trace::Evaluate(t).operator[](
        internal_trace::Evaluate(indices)...);
  }
  template <typename T, typename... Indices>
  using invoke_type =
      decltype(internal_trace::Evaluate(std::declval<T const &>())
                   .
                   operator[](internal_trace::Evaluate(
                       std::declval<Indices const &>())...));
};

struct Invoke {
  static constexpr char name[] = "operator()";
  template <typename T, typename... Arguments>
  static constexpr decltype(auto) invoke(T const &t,
                                         Arguments const &...arguments) {
    return internal_trace::Evaluate(t)(internal_trace::Evaluate(arguments)...);
  }
  template <typename T, typename... Arguments>
  using invoke_type =
      decltype(internal_trace::Evaluate(std::declval<T const &>())(
          internal_trace::Evaluate(std::declval<Arguments const &>())...));
};

// Analogous to the expansion of the macros above, except that `,` cannot be
// passed as a macro argument, so the body is implemented directly outside the
// macro.
struct Comma {
  static constexpr char name[] = "operator,";

  template <typename L, typename R>
  static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {
    return (internal_trace::Evaluate(lhs), internal_trace::Evaluate(rhs));
  }
  template <typename L, typename R>
  using invoke_type =
      decltype((internal_trace::Evaluate(std::declval<L const &>()),
                internal_trace::Evaluate(std::declval<L const &>())));
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_ACTIONS_H
