#ifndef NTH_TRY_INTERNAL_HANDLER_H
#define NTH_TRY_INTERNAL_HANDLER_H

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

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

struct EmptyTryResultType {
  template <nth::precisely<decltype(nullptr)> N>
  constexpr operator N() const {
    return nullptr;
  }

  template <typename U>
  constexpr operator U*() const {
    return nullptr;
  }

  template <nth::precisely<std::nullopt_t> N>
  constexpr operator N() const {
    return std::nullopt;
  }

  template <typename U>
  constexpr operator std::optional<U>() const {
    return std::nullopt;
  }

  template <nth::precisely<bool> B>
  constexpr operator B() const {
    return false;
  }

  friend void NthFormat(auto& w, auto&, EmptyTryResultType) {
    io::write_text(w, "{}");
  }
};

inline constexpr DefaultHandler default_handler;

template <typename T>
struct PointerHandler {
  static constexpr bool okay(T* ptr) { return ptr; }
  static constexpr EmptyTryResultType transform_return(T* ptr) { return {}; }
  static constexpr T& transform_value(T* ptr) { return *ptr; }
};

template <typename T>
inline constexpr PointerHandler<T> pointer_handler;

template <typename T>
struct OptionalHandler {
  static constexpr bool okay(std::optional<T> const& opt) {
    return opt.has_value();
  }
  static constexpr EmptyTryResultType transform_return(
      std::optional<T> const&) {
    return {};
  }
  static constexpr T const& transform_value(std::optional<T> const& opt) {
    return *opt;
  }

  static constexpr T& transform_value(std::optional<T>& opt) { return *opt; }

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

}  // namespace nth

#endif  // NTH_TRY_INTERNAL_HANDLER_H
