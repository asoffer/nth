#ifndef NTH_UTILITY_FUNCTION_REF_H
#define NTH_UTILITY_FUNCTION_REF_H

#include "nth/memory/address.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/concepts/invocable.h"

namespace nth {
namespace internal_function_ref {
template <typename F, typename... Args>
nth::return_type<F, Args...> Invoker(void *obj, Args... args) {
  return (*reinterpret_cast<nth::decayed<F> *>(obj))(args...);
};

}  // namespace internal_function_ref

template <typename>
struct function_ref;
template <typename Ret, typename... Args>
struct function_ref<Ret(Args...)> {
  function_ref() = default;
  function_ref(decltype(nullptr)) : function_ref() {}

  function_ref(Ret (*f)(Args...))
      : data_(nullptr),
        invoker_(reinterpret_cast<Ret (*)(void *, Args...)>(f)) {}

  template <typename F>
  function_ref(F &&f NTH_ATTRIBUTE(lifetimebound)) requires(
      nth::invocable<F, Args...>
          and nth::precisely<nth::return_type<F, Args...>, Ret>)
      : data_(nth::address(f)), invoker_(internal_function_ref::Invoker<F>) {}

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
};

}  // namespace nth

#endif  // NTH_UTILITY_FUNCTION_REF_H
