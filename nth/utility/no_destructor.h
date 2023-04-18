#ifndef NTH_UTILITY_NO_DESTRUCTOR_H
#define NTH_UTILITY_NO_DESTRUCTOR_H

#include <memory>

#include "nth/utility/buffer.h"

namespace nth {
template <typename T>
struct NoDestructor : private buffer<sizeof(T), alignof(T)> {
  template <typename... Args>
  constexpr NoDestructor(Args &&...args)
      : buffer<sizeof(T), alignof(T)>(buffer_construct<T>,
                                      std::forward<Args>(args)...) {}

  T &operator*() & { return this->template as<T>(); }
  T const &operator*() const & { return this->template as<T>(); }
  T &&operator*() && { return std::move(*this).template as<T>(); }
  T const &&operator*() const && { return std::move(*this).template as<T>(); }

  T const *operator->() const { return std::addressof(this->template as<T>()); }
  T *operator->() { return std::addressof(this->template as<T>()); }
};

template <typename T>
NoDestructor(T) -> NoDestructor<T>;

}  // namespace nth

#endif  // NTH_UTILITY_NO_DESTRUCTOR_H
