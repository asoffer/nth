#ifndef NTH_UTILITY_BUFFER_H
#define NTH_UTILITY_BUFFER_H

#include <algorithm>
#include <concepts>
#include <cstddef>

#include "nth/base/core.h"

namespace nth {
namespace internal_buffer {

struct BufferBase {};

template <typename T, size_t Size, size_t Alignment>
concept FitsInBuffer = (sizeof(T) <= Size and alignof(T) <= Alignment);

}  // namespace internal_buffer

template <typename T>
struct buffer_construct_t {};
template <typename T>
inline constexpr buffer_construct_t<T> buffer_construct;

// Constructs a buffer sufficient to hold any value of any type whose size is no
// more than `Size` and alignment divides `Alignment`.
template <size_t Size, size_t Alignment>
struct alignas(Alignment) buffer : internal_buffer::BufferBase {
  static constexpr size_t size      = Size;
  static constexpr size_t alignment = Alignment;

  // Constructs an empty buffer with no value held.
  constexpr buffer() = default;

  // Constructs `T` in the buffer with the given arguments.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T, typename... Args>
  constexpr buffer(buffer_construct_t<T>, Args &&...args) {
    new (buf_) T(NTH_FWD(args)...);
  }

  // Constructs `T` in the buffer with the given arguments. No object may be
  // present in the buffer when `construct` is invoked.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T, int &...,
            typename... Args>
  T &construct(Args &&...args) {
    return *new (buf_) T(NTH_FWD(args)...);
  }

  // Destroys the object of type `T` present in the buffer. Behavior is
  // undefined if an object of another type is present or if no object is
  // present.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  void destroy() {
    this->get<T>()->~T();
  }

  // Returns a reference to the object of type `T` held in the buffer. Behavior
  // is undefined in no object an object of another type is present.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T const &as() const & {
    return *get<T>();
  }

  // Returns a reference to the object of type `T` held in the buffer. Behavior
  // is undefined in no object an object of another type is present.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T &as() & {
    return *get<T>();
  }

  // Returns a reference to the object of type `T` held in the buffer. Behavior
  // is undefined in no object an object of another type is present.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T const &&as() const && {
    return NTH_MOVE(*get<T>());
  }

  // Returns a reference to the object of type `T` held in the buffer. Behavior
  // is undefined in no object an object of another type is present.
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T &&as() && {
    return NTH_MOVE(*get<T>());
  }

 private:
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T *get() {
    return reinterpret_cast<T *>(buf_);
  }
  template <internal_buffer::FitsInBuffer<Size, Alignment> T>
  T const *get() const {
    return reinterpret_cast<T const *>(buf_);
  }

  alignas(Alignment) char buf_[Size];
};

template <>
struct buffer<0, 0> {};

template <typename Buffer, typename... Ts>
concept buffer_type_sufficient_for =
    std::derived_from<Buffer, internal_buffer::BufferBase> and
    (internal_buffer::FitsInBuffer<Ts, Buffer::size, Buffer::alignment> and...);

template <typename... Ts>
using buffer_sufficient_for =
    ::nth::buffer<std::max({sizeof(Ts)...}), std::max({alignof(Ts)...})>;

}  // namespace nth

#endif  // NTH_UTILITY_BUFFER_H
