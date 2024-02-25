#ifndef NTH_UTILITY_EARLY_EXIT_H
#define NTH_UTILITY_EARLY_EXIT_H

#include <concepts>
#include <coroutine>
#include <type_traits>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/meta/type.h"
#include "nth/meta/concepts.h"
#include "nth/utility/unconstructible.h"

namespace nth {
namespace internal_early_exit {

struct promise_return_type_base;
struct early_exit_constructor_t
    : unconstructible_except_by<internal_early_exit::promise_return_type_base> {
};

}  // namespace internal_early_exit

template <typename>
struct early_exitable;

// A concept matching any type that supports early-exiting via `co_await`. A
// type `T` must derive from `early_exitable<T>` and `using` this base-classes
// constructor. The type must also be convertible to bool.
template <typename T>
concept supports_early_exit = std::derived_from<T, early_exitable<T>> and
    (std::is_constructible_v<T, internal_early_exit::early_exit_constructor_t,
                             T*&>and requires(T const& t) {
      static_cast<bool>(t);
    });

// A concept matching early-exit handlers for a type `T` that supports
// early-exiting. The handle must be invocable with a `T const&` or with no
// arguments.
template <typename Handler, typename T>
concept early_exit_handler_for =
    std::is_invocable_v<Handler, T> or std::is_invocable_v<Handler>;

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
struct early_exitable {
  struct promise_type {
    internal_early_exit::promise_return_type<T> get_return_object() {
      return *this;
    }

    static constexpr std::suspend_never initial_suspend() noexcept {
      return {};
    }
    static constexpr std::suspend_never final_suspend() noexcept { return {}; }
    static constexpr void unhandled_exception() noexcept {}

    template <nth::explicitly_convertible_to<T> U>
    void return_value(U&& value);

    template <nth::explicitly_convertible_to<T> U>
    auto await_transform(U&& value);
    template <nth::explicitly_convertible_to<T> U>
    auto await_transform(U const& value);

   private:
    friend internal_early_exit::promise_return_type<T>;
    T* value_;
  };

  early_exitable() = default;
  explicit early_exitable(internal_early_exit::early_exit_constructor_t,
                          T*& v) {
    v = static_cast<T*>(this);
  };

  // TODO: If the type has trivial ABI it ends up being destroyed earlier than
  // desired, and the write in `return_value` will seemingly be undefined
  // behavior. It's unclear whether we're relying on undefined behavior here, or
  // the destructor's triviality is insufficient grounds for choosing to pass
  // the object in registers, making some optimizations unsound (i.e., it's a
  // miscompile).
  ~early_exitable() {}
};

template <typename T>
template <nth::explicitly_convertible_to<T> U>
void early_exitable<T>::promise_type::return_value(U&& value) {
  if constexpr (requires { *value_ = std::forward<U>(value); }) {
    *value_ = std::forward<U>(value);
  } else {
    *value_ = static_cast<T>(std::forward<U>(value));
  }
}

template <typename T>
template <nth::explicitly_convertible_to<T> U>
auto early_exitable<T>::promise_type::await_transform(U&& value) {
  struct awaitable_t {
    bool await_ready() const { return static_cast<bool>(value); }

    decltype(auto) await_resume() const
        requires(requires(U u) { *std::move(u); }) {
      return *std::move(value);
    }
    void await_resume() const requires(not requires(U u) { *std::move(u); }) {}

    void await_suspend(std::coroutine_handle<promise_type> h) {
      h.promise().return_value(std::move(value));
      h.destroy();
    }

    U&& value;
  };
  return awaitable_t{std::move(value)};
}

template <typename T>
template <nth::explicitly_convertible_to<T> U>
auto early_exitable<T>::promise_type::await_transform(U const& value) {
  struct awaitable_t {
    bool await_ready() const { return static_cast<bool>(value); }
    decltype(auto) await_resume() const requires(requires(U const& u) { *u; }) {
      return *value;
    }
    void await_resume() const requires(not requires(U const& u) { *u; }) {}
    void await_suspend(std::coroutine_handle<promise_type> h) {
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
struct on_exit : early_exitable<on_exit<In, Handler>> {
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
  on_exit(In input NTH_ATTRIBUTE(lifetimebound),
          Handler handler NTH_ATTRIBUTE(lifetimebound))
      : input_(std::forward<In>(input)),
        handler_(std::forward<Handler>(handler)) {}

  explicit operator bool() const { return static_cast<bool>(input_); }
  decltype(auto) operator*() const requires(requires { *std::declval<In>(); }) {
    return *std::forward<In>(input_);
  }

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
