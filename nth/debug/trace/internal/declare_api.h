#ifndef NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H
#define NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H

#include "nth/debug/trace/internal/trace.h"

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
    return nth::internal_debug::Traced<                                        \
        Impl<#memfn>, typename std::decay_t<decltype(*this)>::type,            \
        NthTypes...>(::nth::internal_debug::Evaluate(*this), ts...);           \
  }

namespace nth::internal_debug {

template <typename T>
struct DefineTrace {
  static constexpr bool defined = false;
};

template <typename Action, typename... Ts>
requires(DefineTrace<typename Action::template invoke_type<Ts...>>::defined)  //
    struct Traced<Action, Ts...>
    : DefineTrace<typename Action::template invoke_type<Ts...>> {
  using action_type                    = Action;
  static constexpr auto argument_types = nth::type_sequence<Ts...>;

  constexpr Traced(auto const &...ts)
      : DefineTrace<typename action_type::template invoke_type<Ts...>>(
            [&] { return action_type::invoke(ts...); }),
        ptrs_{std::addressof(ts)...} {}

 private:
  friend struct TracedTraversal;

  void const *ptrs_[sizeof...(Ts)];
};

}  // namespace nth::internal_debug

#define NTH_DEBUG_INTERNAL_EXPAND_A(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_B
#define NTH_DEBUG_INTERNAL_EXPAND_B(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_A
#define NTH_DEBUG_INTERNAL_EXPAND_A_END
#define NTH_DEBUG_INTERNAL_EXPAND_B_END
#define NTH_DEBUG_INTERNAL_END(...) NTH_DEBUG_INTERNAL_END_IMPL(__VA_ARGS__)
#define NTH_DEBUG_INTERNAL_END_IMPL(...) __VA_ARGS__##_END

#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(t, member_function_names)         \
  struct nth::internal_debug::DefineTrace<t>                                   \
      : nth::internal_debug::TracedValue<t> {                                  \
   private:                                                                    \
    template <nth::CompileTimeString>                                          \
    struct Impl;                                                               \
                                                                               \
   public:                                                                     \
    using type                                     = t;                        \
    [[maybe_unused]] static constexpr bool defined = true;                     \
    constexpr DefineTrace(auto f)                                              \
        : nth::internal_debug::TracedValue<type>(f) {}                         \
    NTH_DEBUG_INTERNAL_END(NTH_DEBUG_INTERNAL_EXPAND_A member_function_names)  \
  }

#endif  // NTH_DEBUG_TRACE_INTERNAL_DECLARE_API_H
