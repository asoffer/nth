#ifndef NTH_DEBUG_TRACE_H
#define NTH_DEBUG_TRACE_H

#include <concepts>
#include <iomanip>
#include <iostream>
#include <memory>

#include "nth/io/string_printer.h"
#include "nth/io/universal_print.h"
#include "nth/meta/compile_time_string.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace nth {
namespace internal_trace {

struct TracedBase {};

template <::nth::CompileTimeString S>
struct CompileTimeStringType {
  static constexpr ::nth::CompileTimeString string = S;
};

template <typename T>
struct TracedValue : TracedBase {
  using type = T;

 protected:
  constexpr TracedValue(auto f, auto const &...ts) : value_(f(ts...)) {}

  type const &value() const & { return value_; }

 private:
  template <typename U>
  friend decltype(auto) Evaluate(U const &value);

  type value_;
};

template <typename Action, typename... Ts>
struct Traced : TracedValue<typename Action::template invoke_type<Ts...>> {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  Traced(Ts const &...ts)
      : TracedValue<typename action_type::template invoke_type<Ts...>>(
            [&](auto const &...vs) -> decltype(auto) {
              return action_type::invoke(vs...);
            },
            ts...),
        ptrs_{std::addressof(ts)...} {}

 private:
  friend struct TracedTraversal;

  void const *ptrs_[sizeof...(Ts)];
};

template <typename T>
concept IsTraced = std::derived_from<T, TracedBase>;
template <typename T, typename U>
concept TracedEvaluatingTo = std::derived_from<T, TracedValue<U>>;

template <typename S>
struct Identity {
  static constexpr auto name = S::string;

  template <typename T>
  using invoke_type = T;

  template <typename T>
  static constexpr decltype(auto) invoke(T const &t) {
    return t;
  }
};

template <typename T>
decltype(auto) Evaluate(T const &value) {
  if constexpr (::nth::internal_trace::IsTraced<T>) {
    return value.value_;
  } else {
    return value;
  }
}

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
  requires(::nth::internal_trace::IsTraced<L> or                               \
           ::nth::internal_trace::IsTraced<R>) auto                            \
  operator op(L const &lhs, R const &rhs) {                                    \
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

template <typename PreFn, std::invocable<> auto PostFn>
struct Responder {
  bool set(char const *expression,
           ::nth::internal_trace::TracedEvaluatingTo<bool> auto const &b,
           char const *file_name, int line_number) {
    value_ = ::nth::internal_trace::Evaluate(b);
    if (not value_) { PreFn{}(expression, b, file_name, line_number); }
    return value_;
  }

  ~Responder() {
    if (not value_) { PostFn(); }
  }

 private:
  bool value_;
};

template <typename T>
struct DefineTrace {
  static constexpr bool defined = false;
};

template <typename Action, typename... Ts>
requires(DefineTrace<typename Action::template invoke_type<Ts...>>::
             defined) struct Traced<Action, Ts...>
    : DefineTrace<typename Action::template invoke_type<Ts...>> {
  template <typename U>
  constexpr Traced(U &&u)
      : DefineTrace<typename Action::template invoke_type<Ts...>>(
            [&] { return std::forward<U>(u); }) {}
};

struct TracedTraversal {
  template <typename T>
  void operator()(T const &trace) {
    if constexpr (::nth::internal_trace::IsTraced<T>) {
      if constexpr (
          ::nth::type<typename T::action_type>.template is_a<::nth::internal_trace::Identity>()) {
        std::cerr << std::string(indentation, ' ')
                  << ::nth::internal_trace::Evaluate(trace) << " [traced value "
                  << std::quoted(T::action_type::name.data()) << "]\n";
      } else {
        T::argument_types.reduce([&](auto... ts) {
          std::cerr << std::string(indentation, ' ') << T::action_type::name
                    << std::string(
                           40 - indentation - sizeof(T::action_type::name), ' ')
                    << "(= " << ::nth::internal_trace::Evaluate(trace) << ")\n";
          indentation += 2;
          size_t i = 0;
          ((*this)(*static_cast<::nth::type_t<ts> const *>(trace.ptrs_[i++])),
           ...);
          indentation -= 2;
        });
      }
    } else {
      std::string s;
      nth::StringPrinter p(s);
      nth::UniversalPrint(p, trace);
      std::cerr << std::string(indentation, ' ') << s << '\n';
    }
  }

  int indentation = 0;
};

struct Explain {
  void operator()(char const *expression,
                  ::nth::internal_trace::TracedEvaluatingTo<bool> auto const &b,
                  char const *file_name, int line_number) {
    std::cerr << "\033[31m"
                 "NTH_ASSERT failed at "
              << "\033[1m" << file_name << ':' << line_number << ":\033[m\n  "
              << expression << "\n\n";

    TracedTraversal traverser;
    traverser(b);
  }
};

}  // namespace internal_trace

template <::nth::CompileTimeString S, int &..., typename T>
::nth::internal_trace::Traced<
    ::nth::internal_trace::Identity<
        ::nth::internal_trace::CompileTimeStringType<S>>,
    T>
Trace(T const &value) {
  return ::nth::internal_trace::Traced<
      ::nth::internal_trace::Identity<
          ::nth::internal_trace::CompileTimeStringType<S>>,
      T>(value);
}

#define NTH_EXPECT(...)                                                        \
  if (::nth::internal_trace::Responder<::nth::internal_trace::Explain, [] {}>  \
          responder;                                                           \
      responder.set((#__VA_ARGS__), (__VA_ARGS__), __FILE__, __LINE__)) {}

#define NTH_ASSERT(...)                                                        \
  if (::nth::internal_trace::Responder<::nth::internal_trace::Explain,         \
                                       std::abort>                             \
          responder;                                                           \
      responder.set((#__VA_ARGS__), (__VA_ARGS__), __FILE__, __LINE__)) {}

#define NTH_INTERNAL_BODY(memfn)                                               \
  template <nth::CompileTimeString>                                            \
  struct Impl;                                                                 \
  template <>                                                                  \
  struct Impl<#memfn> {                                                        \
    static constexpr char const name[] = #memfn;                               \
    template <typename NthType, typename... NthTypes>                          \
    using invoke_type = decltype(std::declval<NthType const &>().memfn(        \
        std::declval<NthTypes const &>()...));                                 \
                                                                               \
    template <typename NthType, typename... NthTypes>                          \
    static constexpr decltype(auto) invoke(NthType const &t,                   \
                                           NthTypes const &...ts) {            \
      return t.memfn(ts...);                                                   \
    }                                                                          \
  };                                                                           \
                                                                               \
  template <typename... NthTypes>                                              \
  auto memfn(NthTypes const &...ts) const {                                    \
    return nth::internal_trace::Traced<                                        \
        Impl<#memfn>, typename std::decay_t<decltype(*this)>::type,            \
        NthTypes...>(this->value(), ts...);                                    \
  }

#define NTH_INTERNAL_EXPAND_A(x) NTH_INTERNAL_BODY(x) NTH_INTERNAL_EXPAND_B
#define NTH_INTERNAL_EXPAND_B(x) NTH_INTERNAL_BODY(x) NTH_INTERNAL_EXPAND_A
#define NTH_INTERNAL_EXPAND_A_END
#define NTH_INTERNAL_EXPAND_B_END
#define NTH_INTERNAL_END(...) NTH_INTERNAL_END_IMPL(__VA_ARGS__)
#define NTH_INTERNAL_END_IMPL(...) __VA_ARGS__##_END

#define NTH_DECLARE_TRACE_API(t, types)                                        \
  struct nth::internal_trace::DefineTrace<t>                                   \
      : nth::internal_trace::TracedValue<t> {                                  \
    using type                    = t;                                         \
    static constexpr bool defined = true;                                      \
    DefineTrace(auto f) : nth::internal_trace::TracedValue<type>(f) {}         \
    NTH_INTERNAL_END(NTH_INTERNAL_EXPAND_A types)                              \
  }

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_H
