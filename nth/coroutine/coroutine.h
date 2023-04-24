#ifndef NTH_COROUTINE_COROUTINE_H
#define NTH_COROUTINE_COROUTINE_H

#include <coroutine>

#include "nth/meta/type.h"
#include "nth/utility/buffer.h"

namespace nth {
// The `coroutine` class template defined in this header allows users to
// interact with coroutines manually. Users may request values yielded from the
// coroutine via the `get` member function template, and may send values to the
// coroutine via the `send` member function template. Users are responsible for
// ensuring that the types sent and received match from the caller/callee. That
// is, when invoking `send<T>` on a suspended coroutine, the next call to
// `co_await` from the coroutine must be awaiting a value of type `T`.
// Similarly, if a coroutine `co_yield`s a value of type `T`, the caller must be
// requesting a value of type `T` via the `get<T>` member function template.
namespace internal_coroutine {

// Forward declaration of the `promise_type` associated with the `coroutine`
// class template defined below.
template <typename ResultType, typename StorageType>
struct Promise;

// Shared base class for `coroutine` instantiations below with void and non-void
// `result_type`s.
template <typename ResultType, typename StorageType>
struct CoroutineBase {
  CoroutineBase(CoroutineBase const&)            = delete;
  CoroutineBase& operator=(CoroutineBase const&) = delete;

  CoroutineBase(CoroutineBase&& c)
      : handle_(std::exchange(c.handle_, nullptr)) {}
  CoroutineBase& operator=(CoroutineBase&& c) {
    handle_ = std::exchange(c.handle_, nullptr);
  }

  // Executes the coroutine until its next suspension point, converts `value` to
  // type `T`, and provides that value to the coroutine. Behavior is undefined
  // if the next co_awaited type is not `T`.
  template <typename T, std::convertible_to<T> U>
  void send(U&& value) requires(
      ::nth::buffer_type_sufficient_for<StorageType, T>) {
    handle_.resume();
    handle_.promise().storage_.template construct<T>(std::forward<U>(value));
  }

  // Executes the coroutine until its next suspension point and extracts the
  // `co_yielded` value. Behavior is undefined if the `co_yielded` value is not
  // of type `T`.
  template <typename T>
  T get() {
    if constexpr (::nth::type<T> != ::nth::type<void>) {
      ::nth::buffer_sufficient_for<T> buffer;
      handle_.promise().set_buffer(&buffer);
      handle_.resume();
      T result = buffer.template as<T>();
      buffer.template destroy<T>();
      return result;
    } else {
      handle_.resume();
    }
  }

  // Executes the coroutine until the next suspension point. Behavior is
  // undefined if this suspension point is not the final suspension point.
  void complete() { handle_.resume(); }

 protected:
  using promise_type = Promise<ResultType, StorageType>;
  explicit CoroutineBase(std::coroutine_handle<promise_type> handle)
      : handle_(handle) {}

  std::coroutine_handle<promise_type> handle_;
};

}  // namespace internal_coroutine

template <typename ResultType = void, typename StorageType = buffer<0, 0>>
struct coroutine : internal_coroutine::CoroutineBase<ResultType, StorageType> {
  using result_type  = ResultType;
  using promise_type = internal_coroutine::Promise<ResultType, StorageType>;

  coroutine(coroutine const&)            = delete;
  coroutine(coroutine&&)                 = default;

  coroutine& operator=(coroutine const&) = delete;
  coroutine& operator=(coroutine&&)      = default;

  ~coroutine() {
    if (this->handle_) { this->handle_.destroy(); }
  }

  // Returns the value `co_return`ed by the coroutine. Behavior is undefined if
  // `co_return` was not yet executed on the coroutine. This member function is
  // marked `[[nodiscard]]`. If you wish to ensure that the coroutine is
  // completed but do not need the result, call `complete` instead.
  [[nodiscard]] result_type result() requires(::nth::type<result_type> !=
                                              ::nth::type<void>) {
    ::nth::buffer_sufficient_for<result_type> buffer;
    this->handle_.promise().set_buffer(&buffer);
    this->handle_.resume();
    result_type result = buffer.template as<result_type>();
    buffer.template destroy<result_type>();
    return result;
  }

 private:
  friend promise_type;

  explicit coroutine(std::coroutine_handle<promise_type> handle)
      : internal_coroutine::CoroutineBase<result_type, StorageType>(handle) {
    this->handle_.promise().coroutine_ = this;
  }
};

template <typename StorageType>
struct coroutine<void, StorageType>
    : internal_coroutine::CoroutineBase<void, StorageType> {
  using result_type  = void;
  using promise_type = internal_coroutine::Promise<void, StorageType>;

  coroutine(coroutine const&)            = delete;
  coroutine(coroutine&&)                 = default;

  coroutine& operator=(coroutine const&) = delete;
  coroutine& operator=(coroutine&&)      = default;

  ~coroutine() {
    if (this->handle_) { this->handle_.destroy(); }
  }

 private:
  friend promise_type;

  explicit coroutine(std::coroutine_handle<promise_type> handle)
      : internal_coroutine::CoroutineBase<result_type, StorageType>(handle) {
    this->handle_.promise().coroutine_ = this;
  }
};

namespace internal_coroutine {

template <typename T, typename PromiseType>
struct Awaitable {
  explicit Awaitable(PromiseType& promise) : promise_(promise) {}

  constexpr bool await_ready() const { return false; }

  void await_suspend(std::coroutine_handle<>) {}

  T await_resume() const {
    if constexpr (::nth::type<T> != ::nth::type<void>) {
      auto& buffer = promise_.storage_;
      T result     = std::move(buffer).template as<T>();
      buffer.template destroy<T>();
      return result;
    }
  }

 private:
  PromiseType& promise_;
};

struct PromiseBase {
  std::suspend_always initial_suspend() const noexcept { return {}; }
  std::suspend_always final_suspend() const noexcept { return {}; }

  void unhandled_exception() const noexcept {}

  template <typename T>
  std::suspend_always yield_value(T&& value) {
    reinterpret_cast<buffer_sufficient_for<std::decay_t<T>>*>(buffer_)
        ->template construct<std::decay_t<T>>(std::forward<T>(value));
    return {};
  }

  void set_buffer(void* buffer) { buffer_ = buffer; }

 protected:
  void* buffer_;
};

// Implementation of a promise type that `co_return`s a non-void value.
template <typename ResultType, typename StorageType>
struct Promise : PromiseBase {
  using result_type  = ResultType;
  using storage_type = ::nth::buffer_sufficient_for<StorageType, ResultType>;

  template <::nth::Type T>
  auto await_transform(T) {
    return Awaitable<typename T::type, Promise>(*this);
  }

  template <typename R, typename S>
  std::suspend_never await_transform(coroutine<R, S>& c) {
    c.handle_.promise().buffer_ =
        std::exchange(coroutine_->handle_.promise().buffer_, nullptr);
    coroutine_->handle_ = std::exchange(c.handle_, nullptr);
    coroutine_->handle_.resume();
    return {};
  }

  template <typename R, typename S>
  std::suspend_never await_transform(coroutine<R, S>&& c) {
    c.handle_.promise().buffer_ =
        std::exchange(coroutine_->handle_.promise().buffer_, nullptr);
    coroutine_->handle_ = std::exchange(c.handle_, nullptr);
    coroutine_->handle_.resume();
    return {};
  }

  coroutine<result_type, StorageType> get_return_object() {
    return coroutine<result_type, StorageType>(
        std::coroutine_handle<Promise>::from_promise(*this));
  }

  template <typename T>
  void return_value(T&& value) noexcept {
    reinterpret_cast<buffer_sufficient_for<std::decay_t<T>>*>(this->buffer_)
        ->template construct<result_type>(std::forward<T>(value));
  }

 private:
  template <typename, typename>
  friend struct Awaitable;
  template <typename, typename>
  friend struct CoroutineBase;

  friend coroutine<result_type, StorageType>;
  storage_type storage_;
  coroutine<result_type, StorageType>* coroutine_;
};

// Implementation of a promise type that `co_return`s void.
template <typename StorageType>
struct Promise<void, StorageType> : PromiseBase {
  using result_type  = void;
  using storage_type = StorageType;

  template <::nth::Type T>
  auto await_transform(T) {
    return Awaitable<typename T::type, Promise>(*this);
  }

  template <typename R, typename S>
  std::suspend_never await_transform(coroutine<R, S>& c) {
    c.handle_.promise().buffer_ =
        std::exchange(coroutine_->handle_.promise().buffer_, nullptr);
    coroutine_->handle_ = std::exchange(c.handle_, nullptr);
    coroutine_->handle_.resume();
    return {};
  }

  template <typename R, typename S>
  std::suspend_never await_transform(coroutine<R, S>&& c) {
    c.handle_.promise().buffer_ =
        std::exchange(coroutine_->handle_.promise().buffer_, nullptr);
    coroutine_->handle_ = std::exchange(c.handle_, nullptr);
    coroutine_->handle_.resume();
    return {};
  }

  coroutine<void, StorageType> get_return_object() {
    return coroutine<void, StorageType>(
        std::coroutine_handle<Promise>::from_promise(*this));
  }

  void return_void() const noexcept {}

 private:
  template <typename, typename>
  friend struct Awaitable;
  template <typename, typename>
  friend struct CoroutineBase;

  friend coroutine<result_type, StorageType>;

  storage_type storage_;
  coroutine<result_type, StorageType>* coroutine_;
};

}  // namespace internal_coroutine
}  // namespace nth

#endif  // NTH_COROUTINE_COROUTINE_H
