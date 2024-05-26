#ifndef NTH_UTILITY_ANY_INVOCABLE_H
#define NTH_UTILITY_ANY_INVOCABLE_H

#include <memory>
#include <utility>

#include "nth/meta/concepts/core.h"
#include "nth/meta/concepts/invocable.h"

namespace nth {
namespace internal_any_invocable {

template <typename F, typename... Args>
nth::return_type<F, Args...> Invoker(void *obj, Args... args) {
  return (*reinterpret_cast<nth::decayed<F> *>(obj))(args...);
};

template <typename F>
void Destroy(void *obj) {
  delete static_cast<F *>(obj);
};

inline void DoNothing(void *) {}

}  // namespace internal_any_invocable

template <typename>
struct any_invocable;
template <typename Ret, typename... Args>
struct any_invocable<Ret(Args...)> {
  any_invocable() = default;
  any_invocable(std::nullptr_t) : any_invocable() {}

  any_invocable(Ret (*f)(Args...))
      : data_(nullptr),
        invoker_(reinterpret_cast<Ret (*)(void *, Args...)>(f)),
        destroy_(internal_any_invocable::DoNothing) {}

  any_invocable(any_invocable &&f)
      : data_(std::exchange(f.data_, nullptr)),
        invoker_(f.invoker_),
        destroy_(f.destroy_) {}

  any_invocable &operator=(any_invocable &&f) {
    data_    = std::exchange(f.data_, nullptr);
    invoker_ = f.invoker_;
    destroy_ = f.destroy_;
    return *this;
  }

  any_invocable(any_invocable const &)            = delete;
  any_invocable &operator=(any_invocable const &) = delete;

  template <typename F>
  any_invocable(F &&f) requires(
      nth::invocable<F, Args...>
          and nth::precisely<nth::return_type<F, Args...>, Ret>)
      : data_(new nth::decayed<F>(std::forward<F>(f))),
        invoker_(internal_any_invocable::Invoker<nth::decayed<F>>),
        destroy_(internal_any_invocable::Destroy<nth::decayed<F>>) {}

  explicit operator bool() const { return invoker_ != nullptr; }

  constexpr Ret operator()(Args... args) const {
    if (data_) {
      return invoker_(data_, args...);
    } else {
      return reinterpret_cast<Ret (*)(Args...)>(invoker_)(args...);
    }
  }

 private:
  void *data_                      = nullptr;
  Ret (*invoker_)(void *, Args...) = nullptr;
  void (*destroy_)(void *)         = nullptr;
};

}  // namespace nth

#endif  // NTH_UTILITY_ANY_INVOCABLE_H
