#ifndef NTH_TRY_INTERNAL_TRY_H
#define NTH_TRY_INTERNAL_TRY_H

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "nth/base/attributes.h"
#include "nth/base/core.h"
#include "nth/base/macros.h"
#include "nth/memory/address.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"

namespace nth {

template <typename T>
decltype(auto) default_try_exit_handler();

namespace internal_try {

struct DefaultHandler {
  static constexpr bool okay(bool b) { return b; }

  template <typename T>
  static constexpr decltype(auto) transform_return(T&& value) {
    return NTH_FWD(value);
  }

  template <typename T>
  static constexpr decltype(auto) transform_value(T&& value) {
    return NTH_FWD(value);
  }
};

inline constexpr DefaultHandler default_handler;

template <typename T>
struct PointerHandler {
  struct result_type {
    template <typename U>
    constexpr operator U*() const {
      return nullptr;
    }
    constexpr operator bool() const { return false; }
  };
  static constexpr bool okay(T* ptr) { return ptr; }
  static constexpr result_type transform_return(T* ptr) { return {}; }
  static constexpr T& transform_value(T* ptr) { return *ptr; }
};

template <typename T>
inline constexpr PointerHandler<T> pointer_handler;

template <typename T>
struct OptionalHandler {
  static constexpr bool okay(std::optional<T> const& opt) {
    return opt.has_value();
  }
  static constexpr std::optional<T> transform_return(
      std::optional<T> const& opt) {
    return std::nullopt;
  }
  static constexpr T const& transform_value(std::optional<T> const& opt) {
    return *opt;
  }

  static constexpr T const& transform_value(std::optional<T>& opt) {
    return *opt;
  }

  static constexpr T&& transform_value(std::optional<T>&& opt) {
    return *NTH_MOVE(opt);
  }
};

template <typename T>
inline constexpr OptionalHandler<T> optional_handler;

struct AbslStatusHandler {
  static constexpr bool okay(absl::Status const& s) { return s.ok(); }

  static absl::Status transform_return(absl::Status const& s) { return s; }
  static absl::Status transform_return(absl::Status&& s) { return NTH_MOVE(s); }

  static constexpr void transform_value(absl::Status const&) { return; }
};

inline constexpr AbslStatusHandler absl_status_handler;

template <typename T>
struct AbslStatusOrHandler {
  static constexpr bool okay(absl::StatusOr<T> const& s) { return s.ok(); }

  static absl::Status transform_return(absl::StatusOr<T> const& s) {
    return s.status();
  }

  static absl::Status transform_return(absl::StatusOr<T>&& s) {
    return NTH_MOVE(s).status();
  }

  static constexpr T const& transform_value(absl::StatusOr<T> const& s) {
    return *s;
  }

  static constexpr T& transform_value(absl::StatusOr<T>& s) { return *s; }

  static constexpr T&& transform_value(absl::StatusOr<T>&& s) {
    return *NTH_MOVE(s);
  }
};

template <typename T>
inline constexpr AbslStatusOrHandler<T> absl_status_or_handler;

struct MainHandler {
  template <typename T>
  static constexpr bool okay(T const& t) {
    return default_try_exit_handler<T>().okay(t);
  }
  static constexpr int transform_return(auto const&) { return 1; }

  template <typename T>
  static constexpr decltype(auto) transform_value(T const& v) {
    return default_try_exit_handler<T>().transform_value(v);
  }

  template <typename T>
  static constexpr decltype(auto) transform_value(T& v) {
    return default_try_exit_handler<T>().transform_value(v);
  }

  template <typename T>
  static constexpr decltype(auto) transform_value(T&& v) {
    return default_try_exit_handler<T>().transform_value(NTH_MOVE(v));
  }
};

template <typename T, bool LValue, bool RValue>
struct wrap {
  explicit wrap(T&& v) : ptr_(nth::address(v)) {}
  explicit wrap(T& v) : ptr_(nth::address(v)) {}
  static wrap make(T& v) { return wrap(v); }
  static wrap make(T&& v) { return wrap(NTH_MOVE(v)); }

  decltype(auto) transform(auto& handler) const {
    if constexpr (LValue) {
      return handler.transform_value(*ptr_);
    } else if constexpr (RValue) {
      return handler.transform_value(NTH_MOVE(*ptr_));
    }
  }

 private:
  T* ptr_;
};

template <typename T, bool LValue, bool RValue>
struct wrap<T&, LValue, RValue> : wrap<T, LValue, RValue> {};

template <typename T, bool LValue, bool RValue>
struct wrap<T&&, LValue, RValue> : wrap<T, LValue, RValue> {};

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

constexpr auto const& NthDefaultTryExitHandler(type_tag<absl::Status>) {
  return internal_try::absl_status_handler;
}

template <typename T>
constexpr auto const& NthDefaultTryExitHandler(type_tag<absl::StatusOr<T>>) {
  return internal_try::absl_status_or_handler<T>;
}

template <typename T>
constexpr auto const& NthDefaultTryExitHandler(type_tag<T*>) {
  return internal_try::pointer_handler<T>;
}

template <typename T>
constexpr auto const& NthDefaultTryExitHandler(type_tag<std::optional<T>>) {
  return internal_try::optional_handler<T>;
}

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
  (__VA_ARGS__)

#define NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER(...)                              \
  NTH_TRY_INTERNAL_TRY_WITH_HANDLER(                                           \
      (::nth::default_try_exit_handler<                                        \
          std::remove_cvref_t<decltype(__VA_ARGS__)>>()),                      \
      __VA_ARGS__)

#define NTH_TRY_INTERNAL_TRY_WITH_HANDLER(handler, ...)                        \
  (({                                                                          \
     using NthTryType       = decltype((__VA_ARGS__));                         \
     auto&& NthInternalExpr = __VA_ARGS__;                                     \
     if (not handler.okay(NthInternalExpr)) {                                  \
       return handler.transform_return(NthInternalExpr);                       \
     }                                                                         \
     ::nth::internal_try::wrap<                                                \
         NthTryType, nth::lvalue_reference<NthTryType>,                        \
         nth::rvalue_reference<NthTryType>>::make(NTH_FWD(NthInternalExpr));   \
   }).transform(handler))

#endif  // NTH_TRY_INTERNAL_TRY_H
