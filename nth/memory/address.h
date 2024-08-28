#ifndef NTH_MEMORY_ADDRESS_H
#define NTH_MEMORY_ADDRESS_H

#include <cstddef>
#include <type_traits>

#include "nth/base/attributes.h"

namespace nth {

template <typename T>
T* address(T& value) {
  if constexpr (std::is_object_v<T>) {
    return reinterpret_cast<T*>(
        &const_cast<char&>(reinterpret_cast<char const volatile&>(value)));
  } else {
    return &value;
  }
}

template <typename T>
std::byte* raw_address(T& value) {
  return reinterpret_cast<std::byte*>(::nth::address(value));
}

template <typename T>
std::byte const* raw_address(T const& value) {
  return reinterpret_cast<std::byte const*>(::nth::address(value));
}

template <typename T>
std::byte volatile* raw_address(T volatile& value) {
  return reinterpret_cast<std::byte volatile*>(::nth::address(value));
}

template <typename T>
std::byte const volatile* raw_address(T const volatile& value) {
  return reinterpret_cast<std::byte const volatile*>(::nth::address(value));
}

}  // namespace nth

#endif  // NTH_MEMORY_ADDRESS_H
