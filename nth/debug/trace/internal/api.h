#ifndef NTH_DEBUG_TRACE_INTERNAL_API_H
#define NTH_DEBUG_TRACE_INTERNAL_API_H

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "nth/debug/trace/internal/concepts.h"
#include "nth/debug/trace/internal/implementation.h"

namespace nth::debug::internal_trace {

// TODO: Extract into it's own library.
constexpr uint64_t Fnv1a(std::string_view s) {
  uint64_t hash        = 14695981039346656037u;
  constexpr uint64_t p = 1099511628211;
  for (char c : s) { hash = (hash * p) ^ static_cast<uint64_t>(c); }
  return hash;
}

template <typename T>
struct Api;

template <typename Action, typename... Ts>
requires(
    Traceable<Api<
        std::remove_cvref_t<typename Action::template invoke_type<Ts...>>>>)  //
    struct TracedExpr<Action, Ts...>
    : Api<std::remove_cvref_t<typename Action::template invoke_type<Ts...>>> {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;
  constexpr TracedExpr(auto const &...ts)
      : Api<std::remove_cvref_t<
            typename action_type::template invoke_type<Ts...>>>(
            [&] { return action_type::invoke(ts...); }),
        ptrs_{std::addressof(ts)...} {}

 private:
  void const *ptrs_[sizeof...(Ts)];
};

}  // namespace nth::debug::internal_trace

#define NTH_DEBUG_INTERNAL_BODY(memfn)                                         \
 private:                                                                      \
  template <>                                                                  \
  struct Impl<Fnv1a(#memfn)> {                                                 \
    [[maybe_unused]] static constexpr char const name[] = #memfn;              \
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
 public:                                                                       \
  template <int &..., typename... NthTypes>                                    \
  auto memfn(NthTypes const &...ts) const {                                    \
    return ::nth::debug::internal_trace::TracedExpr<                           \
        Impl<Fnv1a(#memfn)>,                                                   \
        typename std::remove_cvref_t<decltype(*this)>::type, NthTypes...>(     \
        ::nth::debug::internal_trace::Evaluate(*this), ts...);                 \
  }

#define NTH_DEBUG_INTERNAL_EXPAND_A(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_B
#define NTH_DEBUG_INTERNAL_EXPAND_B(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_A
#define NTH_DEBUG_INTERNAL_EXPAND_A_END
#define NTH_DEBUG_INTERNAL_EXPAND_B_END
#define NTH_DEBUG_INTERNAL_END(...) NTH_DEBUG_INTERNAL_END_IMPL(__VA_ARGS__)
#define NTH_DEBUG_INTERNAL_END_IMPL(...) __VA_ARGS__##_END

#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(type, member_function_names)      \
  template <>                                                                  \
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_IMPL(type, member_function_names)

#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_TEMPLATE(type,                    \
                                                      member_function_names)   \
  NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_IMPL(type, member_function_names)

#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_IMPL(t, member_function_names)    \
  struct ::nth::debug::internal_trace::Api<t>                                  \
      : ::nth::debug::internal_trace::TracedValue<t> {                         \
   private:                                                                    \
    template <uint64_t>                                                        \
    struct Impl;                                                               \
                                                                               \
   public:                                                                     \
    using NthInternalIsDebugTraceable = void;                                  \
    using type                        = t;                                     \
    constexpr Api(auto f)                                                      \
        : ::nth::debug::internal_trace::TracedValue<type>(f) {}                \
    NTH_DEBUG_INTERNAL_END(NTH_DEBUG_INTERNAL_EXPAND_A member_function_names)  \
  };                                                                           \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE

#endif  // NTH_DEBUG_TRACE_INTERNAL_API_H
