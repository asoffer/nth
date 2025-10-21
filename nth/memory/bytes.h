#ifndef NTH_MEMORY_BYTES_H
#define NTH_MEMORY_BYTES_H

#include <concepts>
#include <cstddef>
#include <span>

#include "nth/base/attributes.h"
#include "nth/memory/address.h"

namespace nth {

// Returns a `std::span<std::byte const>` over the bytes of the object `t`.
template <typename T>
std::span<std::byte const, sizeof(T)> bytes(
    T const& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte const, sizeof(T)>(nth::raw_address(t), sizeof(T));
}

// Returns a `std::span<std::byte>` over the bytes of the object `t`.
template <typename T>
std::span<std::byte, sizeof(T)> bytes(T& t NTH_ATTRIBUTE(lifetimebound)) {
  return std::span<std::byte, sizeof(T)>(nth::raw_address(t), sizeof(T));
}

// Returns a `std::span<std::byte const>` over the bytes ranging from
// `t.begin()` to `t.end()`.
template <typename T>
std::span<std::byte const> byte_range(T const& t NTH_ATTRIBUTE(lifetimebound))
  requires(requires {
    { t.begin() } -> std::contiguous_iterator;
    { t.end() } -> std::contiguous_iterator;
  })
{
  return std::span<std::byte const>(nth::raw_address(*t.begin()),
                                    nth::raw_address(*t.end()));
}

template <typename T, int N>
std::span<std::byte const, N * sizeof(T)> byte_range(
    T const (&buffer NTH_ATTRIBUTE(lifetimebound))[N]) {
  return std::span<std::byte const, N * sizeof(T)>(nth::raw_address(buffer),
                                                   N * sizeof(T));
}

// Returns a `std::span<std::byte>` over the bytes ranging from`t.begin()` to
// `t.end()`.
template <typename T>
std::span<std::byte> byte_range(T& t NTH_ATTRIBUTE(lifetimebound))
  requires(requires {
    { t.begin() } -> std::contiguous_iterator;
    { t.end() } -> std::contiguous_iterator;
  } and not std::is_const_v<std::remove_reference_t<decltype(*t.begin())>>)
{
  return std::span<std::byte>(nth::raw_address(*t.begin()),
                              nth::raw_address(*t.end()));
}

template <typename T, int N>
std::span<std::byte, N * sizeof(T)> byte_range(
    T (&buffer NTH_ATTRIBUTE(lifetimebound))[N]) {
  return std::span<std::byte, N * sizeof(T)>(nth::raw_address(buffer),
                                             N * sizeof(T));
}

}  // namespace nth

#endif  // NTH_MEMORY_BYTES_H
