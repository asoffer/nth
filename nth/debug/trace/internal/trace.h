#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACE_H

#include "nth/base/attributes.h"
#include "nth/base/core.h"
#include "nth/hash/fnv1a.h"
#include "nth/memory/address.h"
#include "nth/meta/compile_time_string.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/concepts/invocable.h"

namespace nth::internal_trace {

struct alignas(2) traced_expression_base {
 protected:
  static traced_expression_base const *set_low_bit(
      traced_expression_base const *ptr) {
    return ptr;
  }
  template <typename T>
  requires(not std::derived_from<T, traced_expression_base>)  //
      static traced_expression_base const *set_low_bit(T const *ptr) {
    return reinterpret_cast<traced_expression_base const *>(
        reinterpret_cast<uintptr_t>(ptr) | uintptr_t{1});
  }
};

template <typename>
struct traced_value_holder;

template <typename T, typename Original = T>
struct traced_members : traced_expression_base {
  using value_type = Original;
};

template <typename T, typename U>
struct traced_members<T &, U> : traced_members<T, U> {};

template <typename T, typename U>
struct traced_members<T const, U> : traced_members<T, U> {};

template <typename T, typename U>
U const &get_value(traced_members<T, U> const &t) {
  return static_cast<U const &>(static_cast<traced_value_holder<U> const &>(t));
}


template <typename T>
struct traced_value_holder : traced_members<T> {
  traced_value_holder(traced_value_holder const &) = delete;
  traced_value_holder(traced_value_holder &&)      = delete;

  constexpr operator T const &() const NTH_ATTRIBUTE(lifetimebound) {
    return value_;
  }

  template <typename F>
  explicit constexpr traced_value_holder(F &&f) : value_(NTH_FWD(f)()) {}

  T value_;
};

template <typename T>
struct traced_value_holder<T &> : traced_members<T &> {
  traced_value_holder(traced_value_holder const &) = delete;
  traced_value_holder(traced_value_holder &&)      = delete;

  constexpr operator T const &() const NTH_ATTRIBUTE(lifetimebound) {
    return *value_;
  }

  explicit constexpr traced_value_holder(T &value)
      : value_(nth::address(value)) {}

 protected:
  T *value_;
};

template <typename T, int DependentCount>
struct traced_expression : traced_value_holder<T> {
  template <typename Op>
  static constexpr traced_expression construct(
      auto const &...arguments NTH_ATTRIBUTE(
          lifetimebound)) requires(sizeof...(arguments) == DependentCount) {
    return traced_expression(
        [&] { return Op::invoke(arguments...); },
        traced_expression_base::set_low_bit(nth::address(arguments))...);
  }

 private:
  template <typename F>
  explicit constexpr traced_expression(F &&f, auto const *...ptrs)
      : traced_value_holder<T>(NTH_FWD(f)), dependents_{ptrs...} {}

  traced_expression_base const *dependents_[DependentCount];
};

template <typename T>
struct traced_expression<T, 0> : traced_value_holder<T> {
  template <typename Op>
  static constexpr traced_expression construct() {
    return traced_expression(Op::invoke());
  }

 private:
  explicit constexpr traced_expression(T const &value)
      : traced_value_holder<T>(value) {}
  explicit constexpr traced_expression(T &&value)
      : traced_value_holder<T>(NTH_MOVE(value)) {}
};

template <std::derived_from<traced_expression_base> T>
typename T::value_type const &traced_value(
    T const &value NTH_ATTRIBUTE(lifetimebound)) {
  return value;
}

template <typename T>
requires(not std::derived_from<T, traced_expression_base>)
    T const &traced_value(T const &value NTH_ATTRIBUTE(lifetimebound)) {
  return value;
}

template <typename T>
using underlying_type =
    decltype(nth::internal_trace::traced_value(nth::value<T>()));

template <compile_time_string S>
struct identity {
  static constexpr char name[] = S;
  template <typename T>
  static constexpr T const &invoke(T const &v NTH_ATTRIBUTE(lifetimebound)) {
    return v;
  }
  template <typename T>
  using invoke_type = T const &;
};

#define NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Op, op)               \
  struct Op {                                                                  \
    static constexpr char name[] =                                             \
        "operator" NTH_STRINGIFY(NTH_IGNORE_PARENTHESES(op));                  \
    template <typename L, typename R>                                          \
    static constexpr decltype(auto) invoke(L const &lhs, R const &rhs) {       \
      return internal_trace::traced_value(lhs) NTH_IGNORE_PARENTHESES(op)      \
          internal_trace::traced_value(rhs);                                   \
    }                                                                          \
    template <typename L, typename R>                                          \
    using invoke_type = decltype((nth::value<L>() NTH_IGNORE_PARENTHESES(op)   \
                                      nth::value<R>()));                       \
  };                                                                           \
                                                                               \
  template <typename L, typename R>                                            \
  requires(                                                                    \
      std::derived_from<L, ::nth::internal_trace::traced_expression_base> or   \
      std::derived_from<                                                       \
          R, ::nth::internal_trace::traced_expression_base>) constexpr auto    \
  operator NTH_IGNORE_PARENTHESES(op)(L const &lhs, R const &rhs) {            \
    return traced_expression<                                                  \
        Op::invoke_type<nth::internal_trace::underlying_type<L>,               \
                        nth::internal_trace::underlying_type<R>>,              \
        2>::template construct<Op>(lhs, rhs);                                  \
  }

NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Le, <=)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Lt, <)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Eq, ==)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Ne, !=)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Ge, >=)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Gt, >)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Cmp, <=>)

NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(And, &)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Or, |)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Xor, ^)

NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Add, +)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Sub, -)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Mul, *)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Div, /)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Mod, %)

NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(LSh, <<)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(RSh, >>)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(ArrowPtr, ->*)
NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR(Comma, (, ))

#undef NTH_TRACE_INTERNAL_DEFINE_TRACED_BINARY_OPERATOR

#define NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Op, op)                \
  struct Op {                                                                  \
    static constexpr char name[] = "operator" NTH_STRINGIFY(op);               \
    template <typename T>                                                      \
    static constexpr decltype(auto) invoke(                                    \
        T const &v NTH_ATTRIBUTE(lifetimebound)) {                             \
      return op internal_trace::traced_value(v);                               \
    }                                                                          \
    template <typename T>                                                      \
    using invoke_type = decltype(op nth::value<T const &>());                  \
  };                                                                           \
                                                                               \
  template <                                                                   \
      std::derived_from<::nth::internal_trace::traced_expression_base> T>      \
  constexpr auto operator op(T const &v NTH_ATTRIBUTE(lifetimebound)) {        \
    return traced_expression<                                                  \
        Op::invoke_type<nth::internal_trace::underlying_type<T>>,              \
        1>::template construct<Op>(v);                                         \
  }

NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Tilde, ~)
NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Not, !)
NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Neg, -)
NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Pos, +)
NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Addr, &)
NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR(Ref, *)

#undef NTH_TRACE_INTERNAL_DEFINE_TRACED_UNARY_OPERATOR

#define NTH_TRACE_INTERNAL_BODY(memfn)                                         \
 private:                                                                      \
  template <>                                                                  \
  struct implementation<::nth::fnv1a(#memfn)> {                                \
    [[maybe_unused]] static constexpr char const name[] = #memfn;              \
    template <typename NthType, typename... NthTypes>                          \
    using invoke_type = decltype(nth::value<NthType const &>().memfn(          \
        nth::value<NthTypes const &>()...));                                   \
                                                                               \
    template <typename NthType, typename... NthTypes>                          \
    static constexpr decltype(auto) invoke(NthType const &t,                   \
                                           NthTypes const &...ts) {            \
      return t.memfn(ts...);                                                   \
    }                                                                          \
  };                                                                           \
                                                                               \
 public:                                                                       \
  template <int &..., typename... NthTypes>                                    \
  constexpr auto memfn(NthTypes const &...ts) const {                          \
    return traced_expression<                                                  \
        typename implementation<::nth::fnv1a(#memfn)>::template invoke_type<   \
            value_type, nth::internal_trace::underlying_type<NthTypes>...>,    \
        1 + sizeof...(ts)>::                                                   \
        template construct<implementation<nth::fnv1a(#memfn)>>(                \
            ::nth::internal_trace::get_value(*this), ts...);                   \
  }

#define NTH_TRACE_INTERNAL_EXPAND_A(x)                                         \
  NTH_TRACE_INTERNAL_BODY(x) NTH_TRACE_INTERNAL_EXPAND_B
#define NTH_TRACE_INTERNAL_EXPAND_B(x)                                         \
  NTH_TRACE_INTERNAL_BODY(x) NTH_TRACE_INTERNAL_EXPAND_A
#define NTH_TRACE_INTERNAL_EXPAND_A_END
#define NTH_TRACE_INTERNAL_EXPAND_B_END
#define NTH_TRACE_INTERNAL_END(...) NTH_TRACE_INTERNAL_END_IMPL(__VA_ARGS__)
#define NTH_TRACE_INTERNAL_END_IMPL(...) __VA_ARGS__##_END

#define NTH_TRACE_INTERNAL_DECLARE_API(type, member_function_names)            \
  NTH_TRACE_INTERNAL_DECLARE_API_IMPL(NTH_IGNORE_PARENTHESES(type),            \
                                      member_function_names)

#define NTH_TRACE_INTERNAL_DECLARE_API_TEMPLATE(type, member_function_names)   \
  NTH_TRACE_INTERNAL_DECLARE_API_IMPL(NTH_IGNORE_PARENTHESES(type),            \
                                      member_function_names)

#define NTH_TRACE_INTERNAL_DECLARE_API_IMPL(t, member_function_names)          \
  template <typename NthInternalTemplateParameter>                             \
  struct ::nth::internal_trace::traced_members<t,                              \
                                               NthInternalTemplateParameter>   \
      : traced_expression_base {                                               \
    using value_type = t;                                                      \
                                                                               \
   private:                                                                    \
    template <uint64_t>                                                        \
    struct implementation;                                                     \
    using NthValueType = t;                                                    \
                                                                               \
   public:                                                                     \
    NTH_TRACE_INTERNAL_END(NTH_TRACE_INTERNAL_EXPAND_A member_function_names)  \
  };                                                                           \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE

}  // namespace nth::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACE_H
