#ifndef NTH_DEBUG_TRACE_INTERNAL_TRACE_H
#define NTH_DEBUG_TRACE_INTERNAL_TRACE_H

#include "nth/base/attributes.h"
#include "nth/base/core.h"
#include "nth/debug/property/internal/concepts.h"
#include "nth/format/format.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/hash/fnv1a.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/address.h"
#include "nth/memory/bytes.h"
#include "nth/meta/compile_time_string.h"
#include "nth/meta/concepts/c_array.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/concepts/invocable.h"

namespace nth::internal_trace {

struct tree_writer {
  struct write_result_type {
    constexpr write_result_type(size_t n) : written_(n) {}
    constexpr size_t written() const { return written_; };

   private:
    size_t written_;
  };

  write_result_type write(std::span<std::byte const> bytes) {
    return write_result_type(bytes.size());
  }

  constexpr void increase_depth() { ++depth_; }
  constexpr void decrease_depth() { --depth_; }
  [[nodiscard]] constexpr size_t depth() const { return depth_; }

 private:
  size_t depth_ = 0;
};

struct traced_expression_base;

struct traversal_vtable {
  void (*write)(tree_writer &, traced_expression_base const &);
};

struct traced_expression_base {
  traced_expression_base(traversal_vtable vtable) : vtable_(vtable) {}

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

 private:
  traversal_vtable vtable_;
};

template <typename T>
struct traced_members : traced_expression_base {
  constexpr traced_members(traversal_vtable vtable)
      : traced_expression_base(vtable) {}

  using NthTraceInternalValueType = T;
};

template <typename T>
struct traced_members<T &> : traced_members<T> {
  constexpr traced_members(traversal_vtable vtable)
      : traced_members<T>(vtable) {}
};

template <typename T>
struct traced_members<T const> : traced_members<T> {
  constexpr traced_members(traversal_vtable vtable)
      : traced_members<T>(vtable) {}
};

template <typename T>
struct traced_value_holder : traced_members<T> {
  traced_value_holder(traced_value_holder const &) = delete;
  traced_value_holder(traced_value_holder &&)      = delete;

  using nth_format_spec = nth::trivial_format_spec;

  friend constexpr auto NthDefaultFormatSpec(
      nth::type_tag<traced_value_holder>) {
    return nth::trivial_format_spec{};
  }

  friend void NthFormat(io::forward_writer auto &w,
                        format_spec<traced_value_holder>,
                        traced_value_holder const &t) {
    nth::interpolate(w, nth::interpolation_spec::from<"{}">(), t.value_);
  }

  constexpr operator T const &() const NTH_ATTRIBUTE(lifetimebound) {
    return value_;
  }

  template <typename F>
  explicit constexpr traced_value_holder(traversal_vtable vtable, F &&f)
      : traced_members<T>(vtable), value_(NTH_FWD(f)()) {}

  T value_;
};

template <typename T>
struct traced_value_holder<T &> : traced_members<T &> {
  traced_value_holder(traced_value_holder const &) = delete;
  traced_value_holder(traced_value_holder &&)      = delete;

  using nth_format_spec = nth::trivial_format_spec;

  friend constexpr auto NthDefaultFormatSpec(
      nth::type_tag<traced_value_holder>) {
    return nth::trivial_format_spec{};
  }

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view,
                                      nth::type_tag<traced_value_holder>) {
    return nth::trivial_format_spec{};
  }

  friend void NthFormat(io::forward_writer auto &w,
                        format_spec<traced_value_holder>,
                        traced_value_holder const &t) {
    nth::interpolate(w, nth::interpolation_spec::from<"{}">(), *t.value_);
  }

  constexpr operator T const &() const NTH_ATTRIBUTE(lifetimebound) {
    return *value_;
  }

  explicit constexpr traced_value_holder(traversal_vtable vtable, T &value)
      : traced_members<T>(vtable), value_(nth::address(value)) {}

 protected:
  T *value_;
};

template <typename T>
T const &get_value(traced_members<T> const &t) {
  return static_cast<T const &>(static_cast<traced_value_holder<T> const &>(t));
}

template <typename T, int DependentCount>
struct traced_expression : traced_value_holder<T> {
  template <typename Op>
  static constexpr traced_expression construct(
      auto const &...arguments NTH_ATTRIBUTE(
          lifetimebound)) requires(sizeof...(arguments) == DependentCount) {
    return traced_expression(
        [&] { return Op::invoke(arguments...); }, Op::name,
        traced_expression_base::set_low_bit(nth::address(arguments))...);
  }

  using nth_format_spec = nth::trivial_format_spec;

  friend constexpr auto NthDefaultFormatSpec(nth::type_tag<traced_expression>) {
    return nth::trivial_format_spec{};
  }

  friend constexpr auto NthFormatSpec(nth::interpolation_string_view,
                                      nth::type_tag<traced_expression>) {
    return nth::trivial_format_spec{};
  }

  friend void NthFormat(tree_writer &w, format_spec<traced_expression> spec,
                        traced_expression const &t) {
    w.write(nth::byte_range(t.name_));
    NthFormat(w, spec, static_cast<traced_value_holder<T> const &>(t));
    w.increase_depth();
    for (int i = 0; i < DependentCount; ++i) {
      NthFormat(w, spec, t.dependents_[i]);
    }
    w.decrease_depth();
  }

 private:
  template <typename F>
  explicit constexpr traced_expression(F &&f, std::string_view name,
                                       auto const *...ptrs)
      : traced_value_holder<T>({.write = erased_write}, NTH_FWD(f)),
        name_(name),
        dependents_{ptrs...} {}

  std::string_view name_;
  traced_expression_base const *dependents_[DependentCount];

  static void erased_write(tree_writer &, traced_expression_base const &) {}
};

template <std::derived_from<traced_expression_base> T>
typename T::NthTraceInternalValueType const &traced_value(
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
  static constexpr std::string_view name = S;
  template <typename T>
  static constexpr T const &invoke(T const &v NTH_ATTRIBUTE(lifetimebound)) {
    return v;
  }
  template <typename T>
  using invoke_type = T const &;
};

struct injector {};

#define NTH_TRACE_INTERNAL_INJECTED_EXPRESSION(...)                            \
  (::nth::debug::internal_trace::TraceInjector{}                               \
       ->*__VA_ARGS__->*::nth::debug::internal_trace::TraceInjector{})

template <typename T>
concept traced_impl =
    std::derived_from<T, nth::internal_trace::traced_expression_base>;

template <typename T>
constexpr decltype(auto) operator->*(injector, T const &value) {
  if constexpr (traced_impl<T>) {
    return value;
  } else {
    if constexpr (nth::c_array<T>) {
      return traced_expression<decltype(value),
                               1>::template construct<identity<"">>(value);
    } else {
      return traced_expression<T const &, 1>::template construct<identity<"">>(
          value);
    }
  }
}

template <typename T>
constexpr decltype(auto) operator->*(T const &value, injector) {
  if constexpr (nth::debug::internal_property::PropertyType<T> or
                traced_impl<T>) {
    return value;
  } else {
    if constexpr (nth::c_array<T>) {
      return traced_expression<decltype(value),
                               1>::template construct<identity<"">>(value);
    } else {
      return traced_expression<T const &, 1>::template construct<identity<"">>(
          value);
    }
  }
}

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
            NthTraceInternalValueType,                                         \
            nth::internal_trace::underlying_type<NthTypes>...>,                \
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
  template <>                                                                  \
  NTH_TRACE_INTERNAL_DECLARE_API_IMPL(NTH_IGNORE_PARENTHESES(type),            \
                                      member_function_names)

#define NTH_TRACE_INTERNAL_DECLARE_API_TEMPLATE(type, member_function_names)   \
  NTH_TRACE_INTERNAL_DECLARE_API_IMPL(NTH_IGNORE_PARENTHESES(type),            \
                                      member_function_names)

#define NTH_TRACE_INTERNAL_DECLARE_API_IMPL(t, member_function_names)          \
  struct ::nth::internal_trace::traced_members<t> : traced_expression_base {   \
    using NthTraceInternalValueType = t;                                       \
                                                                               \
   private:                                                                    \
    template <uint64_t>                                                        \
    struct implementation;                                                     \
                                                                               \
   public:                                                                     \
    NTH_TRACE_INTERNAL_END(NTH_TRACE_INTERNAL_EXPAND_A member_function_names)  \
  };                                                                           \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE

}  // namespace nth::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_TRACE_H
