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
  // Executes the coroutine until its next suspension point, converts `value` to
  // type `T`, and provides that value to the coroutine. Behavior is undefined
  // if the next co_awaited type is not `T`.
  template <typename T, std::convertible_to<T> U>
  void send(U&& value) requires(
      ::nth::buffer_type_sufficient_for<StorageType, T>) {
    handle_.resume();
    static_cast<StorageType&>(handle_.promise())
        .template construct<T>(std::forward<U>(value));
  }

  // Executes the coroutine until its next suspension point and extracts the
  // `co_yielded` value. Behavior is undefined if the `co_yielded` value is not
  // of type `T`.
  template <typename T>
  T get() requires(::nth::buffer_type_sufficient_for<StorageType, T>) {
    handle_.resume();
    auto& buffer =
        static_cast<typename promise_type::storage_type&>(handle_.promise());
    T result     = buffer.template as<T>();
    buffer.template destroy<T>();
    return result;
  }

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

  ~coroutine() {
    // The buffer will only be filled via `co_return`, which means that
    // `handle_.done()` determines whether the result is populated.
    if (this->handle_.done()) {
      static_cast<typename promise_type::storage_type&>(this->handle_.promise())
          .template destroy<result_type>();
    }
    this->handle_.destroy();
  }

  // Returns a reference to the value `co_return`ed by the coroutine stored in
  // the associated promise. Behavior is undefined if `co_return` was not yet
  // executed on the coroutine.
  result_type const& result() const& requires(::nth::type<result_type> !=
                                              ::nth::type<void>) {
    this->handle_.resume();
    return this->handle_.promise().result();
  }

  // Returns a reference to the value `co_return`ed by the coroutine stored in
  // the associated promise. Behavior is undefined if `co_return` was not yet
  // executed on the coroutine.
  result_type&& result() &&
      requires(::nth::type<result_type> != ::nth::type<void>) {
    this->handle_.resume();
    return std::move(this->handle_.promise()).result();
  }

 private:
  friend promise_type;
  using internal_coroutine::CoroutineBase<ResultType,
                                          StorageType>::CoroutineBase;
};

template <typename StorageType>
struct coroutine<void, StorageType>
    : internal_coroutine::CoroutineBase<void, StorageType> {
  using result_type  = void;
  using promise_type = internal_coroutine::Promise<void, StorageType>;

  ~coroutine() { this->handle_.destroy(); }

 private:
  friend promise_type;
  using internal_coroutine::CoroutineBase<void, StorageType>::CoroutineBase;
};

namespace internal_coroutine {

template <typename T, typename PromiseBaseType>
struct Awaitable {
  explicit Awaitable(PromiseBaseType& promise) : promise_(promise) {}

  constexpr bool await_ready() const { return false; }

  void await_suspend(std::coroutine_handle<>) {}

  auto await_resume() const {
    auto& buffer =
        static_cast<typename PromiseBaseType::storage_type&>(promise_);
    T result = std::move(buffer).template as<T>();
    buffer.template destroy<T>();
    return result;
  }

 private:
  PromiseBaseType& promise_;
};

template <typename StorageType>
struct PromiseBase : protected StorageType {
  std::suspend_always initial_suspend() const noexcept { return {}; }
  std::suspend_always final_suspend() const noexcept { return {}; }

  template <::nth::Type T>
  auto await_transform(T) {
    return Awaitable<typename T::type, PromiseBase>(*this);
  }
  void unhandled_exception() const noexcept {}

  template <typename T>
  std::suspend_always yield_value(T&& value) requires(
      ::nth::buffer_type_sufficient_for<StorageType, T>) {
    static_cast<StorageType&>(*this).template construct<std::decay_t<T>>(
        std::forward<T>(value));
    return {};
  }

 private:
  using storage_type = StorageType;

  template <typename, typename>
  friend struct CoroutineBase;

  template <typename, typename>
  friend struct Awaitable;
};

// Implementation of a promise type that `co_return`s a non-void value.
template <typename ResultType, typename StorageType>
struct Promise
    : PromiseBase<::nth::buffer_sufficient_for<StorageType, ResultType>> {
  using result_type  = ResultType;
  using storage_type = ::nth::buffer_sufficient_for<StorageType, ResultType>;

  coroutine<result_type, StorageType> get_return_object() {
    return coroutine<result_type, StorageType>(
        std::coroutine_handle<Promise>::from_promise(*this));
  }

  template <typename T>
  void return_value(T&& value) noexcept {
    static_cast<storage_type&>(*this).template construct<result_type>(
        std::forward<T>(value));
  }

  result_type& result() & {
    return static_cast<storage_type&>(*this).template as<result_type>();
  }
  result_type const& result() const& {
    return static_cast<storage_type const&>(*this).template as<result_type>();
  }
  result_type&& result() && {
    return std::move(static_cast<storage_type&&>(*this))
        .template as<result_type>();
  }
  result_type const&& result() const&& {
    return std::move(static_cast<storage_type const&&>(*this))
        .template as<result_type>();
  }

 private:
  friend coroutine<result_type, StorageType>;
};

// Implementation of a promise type that `co_return`s void.
template <typename StorageType>
struct Promise<void, StorageType> : PromiseBase<StorageType> {
  using result_type = void;
  using storage_type = StorageType;

  coroutine<void, StorageType> get_return_object() {
    return coroutine<void, StorageType>(
        std::coroutine_handle<Promise>::from_promise(*this));
  }

  void return_void() const noexcept {}

 private:
  friend coroutine<result_type, StorageType>;
};

}  // namespace internal_coroutine
}  // namespace nth

#endif  // NTH_COROUTINE_COROUTINE_H
