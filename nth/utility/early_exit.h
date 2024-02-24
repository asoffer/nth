#ifndef NTH_UTILITY_EARLY_EXIT_H
#define NTH_UTILITY_EARLY_EXIT_H

#include <concepts>
#include <coroutine>
#include <type_traits>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/meta/type.h"
#include "nth/utility/unconstructible.h"

namespace nth {
template <typename From, typename To>
concept explicitly_convertible_to = requires(From const& from) {
  static_cast<To>(from);
};

struct early_exit_constructor_t;

// A concept matching any type that supports early-exiting via `co_await`. A
// type `T` must have a constructor accepting an `early_exit_constructor_t` and
// a mutable reference to a pointer to itself. This constructor must assign
// `this` to the reference. The type must also be convertible to bool.
template <typename T>
concept supports_early_exit =
    (std::is_constructible_v<T, early_exit_constructor_t, T*&> and
     requires(T const& t) { static_cast<bool>(t); });

// For any type `T` supporting early-exiting, computes the associated return
// type. If the type can be dereferenced, the type of `*t`, removing any
// CV-reference qualifiers is the associated return type. Otherwise, the
// associated return type is `void`.
template <supports_early_exit T>
inline constexpr auto early_exit_type = [] {
  if constexpr (requires(T const& t) { *t; }) {
    return type<std::remove_cvref_t<decltype(*std::declval<T>())>>;
  } else {
    return type<void>;
  }
}();

// A concept matching early-exit handlers for a type `T` that supports
// early-exiting. The handle must be invocable with a `T const&` or with no
// arguments.
template <typename Handler, typename T>
concept early_exit_handler_for =
    std::is_invocable_v<Handler, T> or std::is_invocable_v<Handler>;

namespace internal_early_exit {
struct promise_return_type_base;
}  // namespace internal_early_exit

struct early_exit_constructor_t
    : unconstructible_except_by<internal_early_exit::promise_return_type_base> {
};

namespace internal_early_exit {

struct promise_return_type_base {
 protected:
  static constexpr early_exit_constructor_t early_exit_constructor = {};
};

template <supports_early_exit T>
struct promise_return_type : promise_return_type_base {
  promise_return_type(typename T::promise_type& promise)
      : pointer_(promise.value_) {}

  promise_return_type(promise_return_type&&)                 = delete;
  promise_return_type(promise_return_type const&)            = delete;
  promise_return_type& operator=(promise_return_type&&)      = delete;
  promise_return_type& operator=(promise_return_type const&) = delete;
  operator T() {
    // TODO: This assumes eager conversion `get_return_object()` to the actual
    // result type.
    return T(promise_return_type_base::early_exit_constructor, pointer_);
  }

 private:
  T*& pointer_;
};

}  // namespace internal_early_exit

template <typename T>
struct early_exit_promise_type {
  internal_early_exit::promise_return_type<T> get_return_object() { return *this; }

  static constexpr std::suspend_never initial_suspend() noexcept { return {}; }
  static constexpr std::suspend_never final_suspend() noexcept { return {}; }
  static constexpr void unhandled_exception() noexcept {}

  template <nth::explicitly_convertible_to<T> U>
  void return_value(U&& value);

  template <nth::explicitly_convertible_to<T> U>
  auto await_transform(U && value);
  template <nth::explicitly_convertible_to<T> U>
  auto await_transform(U const& value);

 private:
  friend internal_early_exit::promise_return_type<T>;
  T* value_;
};

template <typename T>
template <nth::explicitly_convertible_to<T> U>
void early_exit_promise_type<T>::return_value(U&& value) {
  if constexpr (requires { *value_ = std::forward<U>(value); }) {
    *value_ = std::forward<U>(value);
  } else {
    *value_ = static_cast<T>(std::forward<U>(value));
  }
}

template <typename T>
template <nth::explicitly_convertible_to<T> U>
auto early_exit_promise_type<T>::await_transform(U&& value) {
  struct awaitable_t {
    bool await_ready() const { return static_cast<bool>(value); }

    decltype(auto) await_resume() const
        requires(requires(U u) { *std::move(u); }) {
      return *std::move(value);
    }
    void await_resume() const requires(not requires(U u) { *std::move(u); }) {}

    void await_suspend(std::coroutine_handle<early_exit_promise_type> h) {
      h.promise().return_value(std::move(value));
      h.destroy();
    }

    U&& value;
  };
  return awaitable_t{std::move(value)};
}

template <typename T>
template <nth::explicitly_convertible_to<T> U>
auto early_exit_promise_type<T>::await_transform(U const& value) {
  struct awaitable_t {
    bool await_ready() const { return static_cast<bool>(value); }
    decltype(auto) await_resume() const requires(requires(U const& u) { *u; }) {
      return *value;
    }
    void await_resume() const requires(not requires(U const& u) { *u; }) {}
    void await_suspend(std::coroutine_handle<early_exit_promise_type> h) {
      h.promise().return_value(value);
      h.destroy();
    }

    U const& value;
  };
  return awaitable_t{value};
}

namespace internal_early_exit {

template <typename In,
          early_exit_handler_for<std::remove_reference_t<In>> Handler>
struct on_exit {
 private:
  static_assert(std::is_reference_v<In>);
  static_assert(std::is_reference_v<Handler>);

  static constexpr auto out_type = [] {
    if constexpr (std::is_invocable_v<Handler, In const&>) {
      return nth::type<std::invoke_result_t<Handler, In const&>>;
    } else {
      return nth::type<std::invoke_result_t<Handler>>;
    }
  }();

 public:
  using promise_type = early_exit_promise_type<on_exit>;

  on_exit(In input NTH_ATTRIBUTE(lifetimebound),
          Handler handler NTH_ATTRIBUTE(lifetimebound))
      : input_(std::forward<In>(input)),
        handler_(std::forward<Handler>(handler)) {}

  explicit operator bool() const { return static_cast<bool>(input_); }

  explicit constexpr operator nth::type_t<out_type>() const
      requires(out_type != nth::type<void>) {
    return invoke();
  }
  explicit constexpr operator In() const requires(out_type == nth::type<void>) {
    invoke();
    return std::forward<In>(input_);
  }
  template <typename T>
  explicit operator T() const requires(out_type == nth::type<void> and
                                       nth::explicitly_convertible_to<In, T>) {
    invoke();
    return static_cast<T>(std::forward<In>(input_));
  }

 private:
  constexpr decltype(auto) invoke() const {
    if constexpr (std::is_invocable_v<Handler, In const&>) {
      return std::forward<Handler>(handler_)(input_);
    } else {
      return std::forward<Handler>(handler_)();
    }
  }

  In input_;
  Handler handler_;
};

}  // namespace internal_early_exit

template <typename In, typename Handler>
auto on_exit(In&& input NTH_ATTRIBUTE(lifetimebound),
             Handler&& handler NTH_ATTRIBUTE(lifetimebound)) {
  return internal_early_exit::on_exit<decltype(std::forward<In>(input)),
                                      decltype(std::forward<Handler>(handler))>(
      std::forward<In>(input), std::forward<Handler>(handler));
}

}  // namespace nth

#endif  // NTH_UTILITY_EARLY_EXIT_H
