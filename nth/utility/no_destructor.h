#ifndef NTH_UTILITY_NO_DESTRUCTOR_H
#define NTH_UTILITY_NO_DESTRUCTOR_H

#include <utility>

namespace nth {
template <typename T>
struct alignas(T) NoDestructor {
 public:
  template <typename... Args>
  constexpr NoDestructor(Args &&...args) {
    new (buf_) T(std::forward<Args>(args)...);
  }

  T const &operator*() const & { return *get(); }
  T &operator*() & { return *get(); }
  T &&operator*() && { return static_cast<T &&>(*get()); }
  T const &&operator*() const && { return static_cast<T const &&>(*get()); }

  T const *operator->() const { return get(); }
  T *operator->() { return get(); }

 private:
  T *get() { return reinterpret_cast<T *>(buf_); }
  T const *get() const { return reinterpret_cast<T const *>(buf_); }
  alignas(T) char buf_[sizeof(T)];
};

template <typename T>
NoDestructor(T) -> NoDestructor<T>;

}  // namespace nth

#endif  // NTH_UTILITY_NO_DESTRUCTOR_H
