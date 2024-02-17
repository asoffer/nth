#ifndef NTH_UTILITY_ANY_INVOCABLE_H
#define NTH_UTILITY_ANY_INVOCABLE_H

#include <memory>
#include <type_traits>

namespace nth {
namespace internal_any_invocable {

template <typename F, typename... Args>
std::invoke_result_t<F, Args...> Invoker(void *obj, Args... args) {
  return (*reinterpret_cast<std::decay_t<F> *>(obj))(args...);
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

  template <typename F>
  any_invocable(F &&f) requires(std::is_invocable_r_v<Ret, F, Args...>)
      : data_(new std::decay_t<F>(std::forward<F>(f))),
        invoker_(internal_any_invocable::Invoker<std::decay_t<F>>),
        destroy_(internal_any_invocable::Destroy<std::decay_t<F>>) {}

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
