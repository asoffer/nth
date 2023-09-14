#ifndef NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H
#define NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H

#include <type_traits>
#include <utility>

#include "nth/base/macros.h"
#include "nth/debug/trace/internal/implementation.h"
#include "nth/meta/type.h"

#define NTH_DEBUG_INTERNAL_BODY(memfn)                                         \
 private:                                                                      \
  template <>                                                                  \
  struct Impl<#memfn> {                                                        \
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
    return ::nth::debug::internal_trace::Traced<                               \
        Impl<#memfn>, typename std::remove_cvref_t<decltype(*this)>::type,     \
        NthTypes...>(::nth::debug::internal_trace::Evaluate(*this), ts...);    \
  }

namespace nth::debug::internal_trace {

template <typename T>
struct DefineTrace {
  static constexpr bool defined = false;
};

template <typename Action, typename... Ts>
requires(DefineTrace<std::remove_cvref_t<
             typename Action::template invoke_type<Ts...>>>::defined)  //
    struct Traced<Action, Ts...>
    : DefineTrace<
          std::remove_cvref_t<typename Action::template invoke_type<Ts...>>> {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  constexpr Traced(auto const &...ts)
      : DefineTrace<std::remove_cvref_t<
            typename action_type::template invoke_type<Ts...>>>(
            [&] { return action_type::invoke(ts...); }),
        ptrs_{std::addressof(ts)...} {}

 private:
  void const *ptrs_[sizeof...(Ts)];
};

}  // namespace nth::debug::internal_trace

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
  struct ::nth::debug::internal_trace::DefineTrace<t>                          \
      : ::nth::debug::internal_trace::TracedValue<t> {                         \
   private:                                                                    \
    template <nth::CompileTimeString>                                          \
    struct Impl;                                                               \
                                                                               \
   public:                                                                     \
    using type                                     = t;                        \
    [[maybe_unused]] static constexpr bool defined = true;                     \
    constexpr DefineTrace(auto f)                                              \
        : ::nth::debug::internal_trace::TracedValue<type>(f) {}                \
    NTH_DEBUG_INTERNAL_END(NTH_DEBUG_INTERNAL_EXPAND_A member_function_names)  \
  };                                                                           \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE

#endif  // NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H
