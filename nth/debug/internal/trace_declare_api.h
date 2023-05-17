#ifndef NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_H
#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_H

#include "nth/debug/internal/trace.h"

#define NTH_DEBUG_INTERNAL_BODY(memfn)                                         \
 private:                                                                      \
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
 public:                                                                       \
  template <typename... NthTypes>                                              \
  auto memfn(NthTypes const &...ts) const {                                    \
    return nth::internal_trace::Traced<                                        \
        Impl<#memfn>, typename std::decay_t<decltype(*this)>::type,            \
        NthTypes...>(this->value(), ts...);                                    \
  }

#define NTH_DEBUG_INTERNAL_EXPAND_A(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_B
#define NTH_DEBUG_INTERNAL_EXPAND_B(x)                                         \
  NTH_DEBUG_INTERNAL_BODY(x) NTH_DEBUG_INTERNAL_EXPAND_A
#define NTH_DEBUG_INTERNAL_EXPAND_A_END
#define NTH_DEBUG_INTERNAL_EXPAND_B_END
#define NTH_DEBUG_INTERNAL_END(...) NTH_DEBUG_INTERNAL_END_IMPL(__VA_ARGS__)
#define NTH_DEBUG_INTERNAL_END_IMPL(...) __VA_ARGS__##_END

#define NTH_DEBUG_INTERNAL_TRACE_DECLARE_API(t, member_function_names)         \
  struct nth::internal_trace::DefineTrace<t>                                   \
      : nth::internal_trace::TracedValue<t> {                                  \
   private:                                                                    \
    template <nth::CompileTimeString>                                          \
    struct Impl;                                                               \
                                                                               \
   public:                                                                     \
    using type                    = t;                                         \
    static constexpr bool defined = true;                                      \
    DefineTrace(auto f) : nth::internal_trace::TracedValue<type>(f) {}         \
    NTH_DEBUG_INTERNAL_END(NTH_DEBUG_INTERNAL_EXPAND_A member_function_names)  \
  }

#endif  // NTH_DEBUG_INTERNAL_TRACE_DECLARE_API_H
