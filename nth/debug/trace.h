#ifndef NTH_DEBUG_TRACE_H
#define NTH_DEBUG_TRACE_H

#include <concepts>
#include <iomanip>
#include <iostream>
#include <memory>

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
concept IsTraced = std::derived_from<T, TracedBase>;

template <typename T, auto U>
concept TracedEvaluatingTo = IsTraced<T> and U == ::nth::type<typename T::type>;

template <typename Action, typename... Ts>
struct Traced : TracedBase {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;
  using type = typename Action::template invoke_type<Ts...>;

  Traced(Ts const &...ts) : ptrs_{std::addressof(ts)...} {}

  // private:
  template <typename T>
  friend decltype(auto) Evaluate(T const &value);

  decltype(auto) value() const {
    return nth::index_sequence<sizeof...(Ts)>.reduce([&](auto... ns) {
      return Action::invoke((*static_cast<Ts const *>(ptrs_[ns]))...);
    });
  };

  void const *ptrs_[sizeof...(Ts)];
};

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

template <typename S, typename T>
struct Traced<Identity<S>, T> : TracedBase {
  using action_type                    = Identity<S>;
  static constexpr auto argument_types = nth::type_sequence<>;
  using type                           = T;

  template <typename U>
  constexpr Traced(U &&u) : value_(std::forward<U>(u)) {}

  template <typename U>
  friend decltype(auto) Evaluate(U const &value);

  type const &value() const { return value_; };

  T value_;
};

template <typename T>
decltype(auto) Evaluate(T const &value) {
  if constexpr (::nth::internal_trace::IsTraced<T>) {
    return value.value();
  } else {
    return value;
  }
}

#define NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Op, op)                      \
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
                         std::declval<L const &>()));                          \
  };                                                                           \
  template <typename L, typename R>                                            \
  requires(::nth::internal_trace::IsTraced<L> or IsTraced<R>) auto             \
  operator op(L const &lhs, R const &rhs) {                                    \
    return Traced<Op, L, R>(lhs, rhs);                                         \
  }

NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Le, <=)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Lt, <)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Eq, ==)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Ne, !=)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Ge, >=)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Gt, >)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Cmp, <=>)

NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(And, &)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Or, |)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Xor, ^)

NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Add, +)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Sub, -)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Mul, *)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Div, /)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(Mod, %)

NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(LSh, <<)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(RSh, >>)
NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR(ArrowPtr, ->*)

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

#undef NTH_TRACE_INTERNAL_DEFINE_BINARY_OPERATOR

#define NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Op, op)                \
  struct Op {                                                                  \
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

NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Tilde, ~)
NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Not, !)
NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Neg, -)
NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Pos, +)
NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Addr, &)
NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR(Ref, *)

#undef NTH_TRACE_INTERNAL_DEFINE_PREFIX_UNARY_OPERATOR

template <typename PreFn, std::invocable<> auto PostFn>
struct Responder {
  bool set(
      char const *expression,
      ::nth::internal_trace::TracedEvaluatingTo<::nth::type<bool>> auto const
          &b,
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

struct TracedTraversal {
  template <typename T>
  void operator()(T const &trace) {
    if constexpr (::nth::internal_trace::IsTraced<T>) {
      if constexpr (
          ::nth::type<typename T::action_type>.template is_a<::nth::internal_trace::Identity>()) {
        std::cerr << std::string(indentation, ' ') << trace.value()
                  << " [traced value "
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
      std::cerr << std::string(indentation, ' ') << trace << '\n';
    }
  }

  int indentation = 0;
};

struct Explain {
  void operator()(
      char const *expression,
      ::nth::internal_trace::TracedEvaluatingTo<::nth::type<bool>> auto const
          &b,
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

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_H
