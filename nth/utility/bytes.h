#ifndef NTH_UTILITY_BYTES_H
#define NTH_UTILITY_BYTES_H

#include <concepts>
#include <cstddef>
#include <memory>
#include <span>

#include "nth/base/attributes.h"

namespace nth {

// Returns a `std::span<std::byte const>` over the bytes of the object `t`.
template <typename T>
std::span<std::byte const, sizeof(T)> bytes(T const& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte const, sizeof(T)>(
      reinterpret_cast<std::byte const*>(std::addressof(t)), sizeof(T));
}

// Returns a `std::span<std::byte>` over the bytes of the object `t`.
template <typename T>
std::span<std::byte, sizeof(T)> bytes(T& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte, sizeof(T)>(
      reinterpret_cast<std::byte*>(std::addressof(t)), sizeof(T));
}

// Returns a `std::span<std::byte const>` over the bytes ranging from
// `t.begin()` to `t.end()`.
template <typename T>
std::span<std::byte const> byte_range(
    T const& t NTH_ATTRIBUTE(lifetimebound)) requires(requires {
  { t.begin() } -> std::contiguous_iterator;
  { t.end() } -> std::contiguous_iterator;
}) {
  return std::span<std::byte const>(
      reinterpret_cast<std::byte const*>(std::addressof(*t.begin())),
      reinterpret_cast<std::byte const*>(std::addressof(*t.end())));
}

// Returns a `std::span<std::byte>` over the bytes ranging from`t.begin()` to
// `t.end()`.
template <typename T>
std::span<std::byte> byte_range(T& t NTH_ATTRIBUTE(lifetimebound)) requires(
    requires {
      { t.begin() } -> std::contiguous_iterator;
      { t.end() } -> std::contiguous_iterator;
    }) {
  return std::span<std::byte>(
      reinterpret_cast<std::byte*>(std::addressof(*t.begin())),
      reinterpret_cast<std::byte*>(std::addressof(*t.end())));
}

}  // namespace nth

#endif  // NTH_UTILITY_BYTES_H
