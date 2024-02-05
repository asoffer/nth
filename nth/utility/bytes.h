#ifndef NTH_UTILITY_BYTES_H
#define NTH_UTILITY_BYTES_H

#include <cstddef>
#include <memory>
#include <span>

#include "nth/base/attributes.h"

namespace nth {

template <typename T>
std::span<std::byte const, sizeof(T)> bytes(T const& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte const, sizeof(T)>(
      reinterpret_cast<std::byte const*>(std::addressof(t)), sizeof(T));
}

template <typename T>
std::span<std::byte, sizeof(T)> bytes(T& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte, sizeof(T)>(
      reinterpret_cast<std::byte*>(std::addressof(t)), sizeof(T));
}

}  // namespace nth

#endif  // NTH_UTILITY_BYTES_H
