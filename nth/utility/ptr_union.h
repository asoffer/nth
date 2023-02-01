#ifndef ICARUS_BASE_PTR_UNION_H
#define ICARUS_BASE_PTR_UNION_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>

#include "nth/meta/concepts.h"
#include "nth/meta/type.h"

namespace nth {
namespace internal_utility {

template <typename T, typename... Ts>
concept const_one_of =
    not nth::any_of<T, Ts...> and nth::any_of<T const, Ts const...>;
template <typename T, typename... Ts>
concept const_ptr_one_of = not nth::any_of<T, Ts...> and
                           nth::any_of<std::remove_pointer_t<T> const *, Ts...>;

}  // namespace internal_utility

// Represents a pointer to any one the template parameter types.
template <typename... Ts>
struct PtrUnion {
  static_assert((1 << (std::min({alignof(Ts)...}) - 1)) >= sizeof...(Ts));
  static constexpr size_t required_alignment = std::min({alignof(Ts)...});

  template <nth::any_of<Ts *...> T>
  PtrUnion(T ptr)
      : ptr_(reinterpret_cast<uintptr_t>(ptr) |
             index<std::remove_pointer_t<T>>()) {}

  template <internal_utility::const_ptr_one_of<Ts *...> T>
  PtrUnion(T ptr)
      : ptr_(reinterpret_cast<uintptr_t>(ptr) |
             index<std::remove_pointer_t<T>>()) {}

  // Returns the pointer of type `T const *` if `*this` currently holds such a
  // pointer, otherwise behavior is undefined.
  template <internal_utility::const_one_of<Ts...> T>
  T const *get() const {
    assert((ptr_ & Mask) == index<T>());
    return reinterpret_cast<T const *>(ptr_ & ~Mask);
  }

  // Returns the pointer of type `T *` if `*this` currently holds such a
  // pointer, otherwise behavior is undefined.
  template <nth::any_of<Ts...> T>
  T *get() {
    assert((ptr_ & Mask) == index<T>());
    return reinterpret_cast<T *>(ptr_ & ~Mask);
  }

  // Returns the pointer of type `T const*` if `*this` currently holds such a
  // pointer, otherwise returns a null pointer.
  template <internal_utility::const_one_of<Ts...> T>
  T const *get_if() const {
    return (ptr_ & Mask) == index<T>()
               ? reinterpret_cast<T const *>(ptr_ & ~Mask)
               : nullptr;
  }

  // Returns the pointer of type `T*` if `*this` currently holds such a pointer,
  // otherwise returns a null pointer.
  template <nth::any_of<Ts...> T>
  T *get_if() {
    return (ptr_ & Mask) == index<T>() ? reinterpret_cast<T *>(ptr_ & ~Mask)
                                       : nullptr;
  }

  template <typename H>
  friend H AbslHashValue(H h, PtrUnion p) {
    return H::combine(std::move(h), p.ptr_);
  }

  // Returns true if and only ifthe two pointers point to the same address and
  // are of the same type.
  friend constexpr bool operator==(PtrUnion lhs, PtrUnion rhs) {
    return lhs.ptr_ == rhs.ptr_;
  }

  // Returns false if and only if the two pointers point to the same address and
  // are of the same type.
  friend constexpr bool operator!=(PtrUnion lhs, PtrUnion rhs) {
    return not(lhs == rhs);
  }

 private:
  static constexpr uintptr_t Mask =
      static_cast<uintptr_t>(required_alignment - 1);

  template <typename T>
  static constexpr uintptr_t index() {
    uintptr_t i      = 0;
    uintptr_t result = 0;
    ((nth::type<T const> == nth::type<Ts const> ? result = i : 0, ++i), ...);
    return result;
  }

  uintptr_t ptr_;
};

}  // namespace nth

#endif  // ICARUS_BASE_PTR_UNION_H
