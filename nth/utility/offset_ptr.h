#ifndef NTH_UTILITY_OFFSET_PTR_H
#define NTH_UTILITY_OFFSET_PTR_H

#include <cstdint>
#include <type_traits>
#include <utility>

namespace nth {

template <typename T>
struct offset_ptr {
  using element_type = T;
  using pointer      = T *;

  offset_ptr() : offset_(to_int(this)) {}
  offset_ptr(decltype(nullptr)) : offset_ptr() {}
  explicit offset_ptr(pointer ptr) : offset_(to_int(this) - to_int(ptr)) {}
  offset_ptr(offset_ptr const &ptr) : offset_ptr(ptr.get()) {}

  offset_ptr &operator=(decltype(nullptr)) {
    offset_ = to_int(this);
    return *this;
  }

  offset_ptr &operator=(pointer ptr) {
    offset_ = to_int(this) - to_int(ptr);
    return *this;
  }

  offset_ptr &operator=(offset_ptr const &ptr) {
    offset_ = to_int(this) - to_int(&ptr) + ptr.offset_;
    return *this;
  }

  friend bool operator==(offset_ptr const &ptr, decltype(nullptr)) {
    return ptr.get() == nullptr;
  }
  friend bool operator==(decltype(nullptr), offset_ptr const &ptr) {
    return ptr.get() == nullptr;
  }
  friend bool operator==(offset_ptr const &lhs, pointer rhs) {
    return lhs.get() == rhs;
  }
  friend bool operator==(pointer lhs, offset_ptr const &rhs) {
    return lhs == rhs.get();
  }
  friend bool operator==(offset_ptr const &lhs, offset_ptr const &rhs) {
    return lhs.get() == rhs.get();
  }

  template <typename H>
  H AbslHashValue(H h, offset_ptr const &p) {
    return H::combine(std::move(h), p.get());
  }

  pointer get() const { return to_ptr(to_int(this) - offset_); }
  pointer operator->() const { return get(); }
  decltype(auto) operator*() const { return *get(); }

 private:
  static constexpr uintptr_t to_int(void const *ptr) {
    return reinterpret_cast<uintptr_t>(ptr);
  }

  static constexpr pointer to_ptr(uintptr_t ptr) {
    if constexpr (std::is_const_v<element_type>) {
      return static_cast<pointer>(reinterpret_cast<void const *>(ptr));
    } else {
      return static_cast<pointer>(reinterpret_cast<void *>(ptr));
    }
  }
  uintptr_t offset_;
};

}  // namespace nth

#endif  // NTH_UTILITY_OFFSET_PTR_H
