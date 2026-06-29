#ifndef NTH_TRY_INTERNAL_TRY_H
#define NTH_TRY_INTERNAL_TRY_H

#include <cstdlib>
#include <optional>

#include "nth/base/attributes.h"
#include "nth/base/core.h"
#include "nth/base/macros.h"
#include "nth/debug/log/log.h"
#include "nth/memory/address.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"
#include "nth/try/internal/handler.h"

namespace nth {
namespace internal_try {

void MaybeLogWithFormat(
    auto&& handler, auto&& arg,
    nth::source_location loc = nth::source_location::current()) {
  using return_type = decltype(handler.transform_return(NTH_FWD(arg)));
  if constexpr (nth::formattable_with<return_type, nth::io::string_writer,
                                      nth::default_formatter_t>) {
    NTH_LOG("FATAL ERROR: {}") <<=
        {nth::log_configuration().source_location(loc),
         handler.transform_return(NTH_FWD(arg))};
  } else {
    NTH_LOG("FATAL ERROR: Unformattable object of type {}") <<=
        {nth::log_configuration().source_location(loc), nth::type<return_type>};
  }
}

template <typename T, bool LValue, bool RValue>
struct wrap;

template <typename T, bool RValue>
struct wrap<T&, true, RValue> {
  explicit wrap(T& v) : ptr_(nth::address(v)) {}
  static wrap make(T& v) { return wrap(v); }

  decltype(auto) transform(auto& handler) const {
    return handler.transform_value(*ptr_);
  }

 private:
  T* ptr_;
};

template <typename T>
struct wrap<T&&, false, true> {
  explicit wrap(T&& v) : v_(NTH_MOVE(v)) {}
  static wrap make(T&& v) { return wrap(NTH_MOVE(v)); }

  decltype(auto) transform(auto& handler) {
    return handler.transform_value(NTH_MOVE(v_));
  }

 private:
  T v_;
};

template <typename T>
struct wrap<T, false, false> {
  explicit wrap(T&& v) : v_(NTH_MOVE(v)) {}
  static wrap make(T&& v) { return wrap(NTH_MOVE(v)); }

  decltype(auto) transform(auto& handler) {
    return handler.transform_value(NTH_MOVE(v_));
  }

 private:
  T v_;
};

}  // namespace internal_try

template <typename T>
decltype(auto) default_try_exit_handler() {
  if constexpr (requires { NthDefaultTryExitHandler(nth::type<T>); }) {
    return NthDefaultTryExitHandler(nth::type<T>);
  } else {
    return internal_try::default_handler;
  }
}

}  // namespace nth

#define NTH_TRY_INTERNAL_TRY(...)                                              \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_TRY_INTERNAL_TRY_WITH_HANDLER,                                    \
         NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER)                                 \
  (NTH_TRY_INTERNAL_RETURN, __VA_ARGS__)

#define NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER(action, ...)                      \
  NTH_TRY_INTERNAL_TRY_WITH_HANDLER(                                           \
      action,                                                                  \
      (::nth::default_try_exit_handler<                                        \
          std::remove_cvref_t<decltype(__VA_ARGS__)>>()),                      \
      __VA_ARGS__)

#define NTH_TRY_INTERNAL_RETURN(handler)                                       \
  return handler.transform_return(NTH_FWD(NthInternalExpr));

#define NTH_TRY_INTERNAL_TRY_WITH_HANDLER(action, handler, ...)                \
  (({                                                                          \
     using NthTryType = decltype((__VA_ARGS__));                               \
     std::conditional_t<nth::rvalue_reference<NthTryType>,                     \
                        std::remove_reference_t<NthTryType>, NthTryType>       \
         NthInternalExpr = __VA_ARGS__;                                        \
     if (not handler.okay(NthInternalExpr)) { action(handler); }               \
     ::nth::internal_try::wrap<                                                \
         NthTryType, nth::lvalue_reference<NthTryType>,                        \
         nth::rvalue_reference<NthTryType>>::make(NTH_FWD(NthInternalExpr));   \
   }).transform(handler))

#define NTH_TRY_INTERNAL_ACTION_return(handler) return
#define NTH_TRY_INTERNAL_ACTION_break(handler) break
#define NTH_TRY_INTERNAL_ACTION_continue(handler) continue
#define NTH_TRY_INTERNAL_ACTION_co_return(handler) co_return

#define NTH_TRY_INTERNAL_ACTION(action)                                        \
  NTH_CONCATENATE(NTH_TRY_INTERNAL_ACTION_, action)

#define NTH_TRY_INTERNAL_UNWRAP_OR(action, ...)                                \
  NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER(NTH_TRY_INTERNAL_ACTION(action),        \
                                       __VA_ARGS__)

#define NTH_TRY_INTERNAL_UNWRAP(...)                                           \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_TRY_INTERNAL_UNWRAP_WITH_HANDLER,                                 \
         NTH_TRY_INTERNAL_UNWRAP_WITHOUT_HANDLER)                              \
  (__VA_ARGS__)

#define NTH_TRY_INTERNAL_UNWRAP_WITHOUT_HANDLER(...)                           \
  NTH_TRY_INTERNAL_UNWRAP_WITH_HANDLER(                                        \
      (::nth::default_try_exit_handler<                                        \
          std::remove_cvref_t<decltype(__VA_ARGS__)>>()),                      \
      __VA_ARGS__)

#define NTH_TRY_INTERNAL_UNWRAP_WITH_HANDLER(handler, ...)                     \
  (({                                                                          \
     using NthTryType       = decltype((__VA_ARGS__));                         \
     auto&& NthInternalExpr = __VA_ARGS__;                                     \
     if (not handler.okay(NthInternalExpr)) {                                  \
       nth::internal_try::MaybeLogWithFormat(handler,                          \
                                             NTH_FWD(NthInternalExpr));        \
       std::abort();                                                           \
     }                                                                         \
     ::nth::internal_try::wrap<                                                \
         NthTryType, nth::lvalue_reference<NthTryType>,                        \
         nth::rvalue_reference<NthTryType>>::make(NTH_FWD(NthInternalExpr));   \
   }).transform(handler))

#endif  // NTH_TRY_INTERNAL_TRY_H
