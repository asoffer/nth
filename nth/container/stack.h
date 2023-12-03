#ifndef NTH_CONTAINER_STACK_H
#define NTH_CONTAINER_STACK_H

#include <concepts>
#include <cstddef>
#include <memory>
#include <new>
#include <utility>

#include "nth/debug/debug.h"

namespace nth {

template <typename T>
struct stack {
  using value_type      = T;
  using size_type       = size_t;
  using reference       = T&;
  using const_reference = T const&;

  stack();
  stack(stack const& s);
  stack(stack&& s);
  stack& operator=(stack&& s);
  stack& operator=(stack const& s);
  ~stack() requires(not std::is_trivially_destructible_v<value_type>);
  ~stack() requires(std::is_trivially_destructible_v<value_type>);

  // Pushes `value` onto the top of the stack.
  void push(value_type const& value);
  void push(value_type&& value);

  // Constructs a new element on the top of the stack
  template <typename... Args>
  requires std::constructible_from<value_type, Args...>
  decltype(auto) emplace(Args&&... args);

  // Removes the top element from the stack. Requires that the stack not be
  // empty.
  void pop();

  // Returns a reference to the most-recently pushed element in the stack.
  // Requires that the stack not be empty.
  reference top() { return *(next_ - 1); }
  const_reference top() const { return *(next_ - 1); }

  // Swaps this object with `other`.
  void swap(stack& other);

  // Returns the number of elements present in the stack.
  size_type size() const { return capacity() - left_; }

  // Returns whether the stack contains no elements.
  bool empty() const { return size() == 0; }

 private:
  size_type* round_up_for_capacity(void* ptr) {
    // TODO: This can be technically undefined behavior because this isn't
    // actually the length of the buffer.
    size_t width = 2 * sizeof(size_type);
    return static_cast<size_type*>(
        std::align(alignof(size_type), sizeof(size_type), ptr, width));
  }

  size_type* capacity_address() { return round_up_for_capacity(next_ + left_); }
  size_type const* capacity_address() const {
    return const_cast<stack*>(this)->capacity_address();
  }

  size_type capacity() const { return *capacity_address(); }

  static consteval std::align_val_t alignment() {
    return alignof(value_type) > alignof(size_type)
               ? std::align_val_t{alignof(value_type)}
               : std::align_val_t{alignof(size_type)};
  }

  static consteval size_type min_alloc_size() {
    return buffer_size_from_capacity(1);
  }

  static constexpr size_type capacity_offset(size_type buffer_size) {
    return (buffer_size & ~size_type{alignof(size_type) - 1}) -
           sizeof(size_type);
  }

  static constexpr size_type buffer_size_from_capacity(size_type cap) {
    return ((((cap * sizeof(value_type) - 1) | (alignof(size_type) - 1)) +
             sizeof(size_type)) |
            (alignof(value_type) - 1)) +
           1;
  }

  void Reallocate() {
    size_type cap         = capacity();
    size_type buffer_size = buffer_size_from_capacity(cap);
    void* new_alloc       = ::operator new(2 * buffer_size, alignment());
    value_type* new_ptr   = static_cast<value_type*>(new_alloc);
    value_type* old_start = next_ - cap;
    for (value_type* p = old_start; p != next_; ++p) {
      new_ptr = 1 + new (new_ptr) value_type(static_cast<value_type&&>(*p));
      p->~value_type();
    }
    ::operator delete(old_start, alignment());

    next_ = new_ptr;
    left_ = capacity_offset(2 * buffer_size) / sizeof(value_type) - cap;
    *capacity_address() = capacity_offset(2 * buffer_size) / sizeof(value_type);
  }

  T* next_;
  size_type left_;
};

template <typename T>
stack<T>::stack()
    : next_(static_cast<value_type*>(
          ::operator new(min_alloc_size(), alignment()))),
      left_((capacity_offset(min_alloc_size()) / sizeof(value_type))) {
  *capacity_address() = left_;
}

template <typename T>
stack<T>::stack(stack const& s)
    : next_(static_cast<value_type*>(
          ::operator new(buffer_size_from_capacity(s.capacity()), alignment()))),
      left_(s.left_) {
  value_type const* e = s.next_;
  for (value_type const* b = s.next_ - s.size(); b != e; ++b) {
    next_ = 1 + new (next_) value_type(*b);
  }
  *capacity_address() = s.capacity();
}

template <typename T>
stack<T>::stack(stack&& s)
    : next_(std::exchange(s.next_, nullptr)),
      left_(std::exchange(s.left_, 0)) {}

template <typename T>
stack<T>& stack<T>::operator=(stack&& s) {
  next_ = std::exchange(s.next_, nullptr);
  left_ = std::exchange(s.left_, 0);
  return *this;
}

template <typename T>
stack<T>& stack<T>::operator=(stack const& s) {
  next_ = static_cast<value_type*>(
      ::operator new(buffer_size_from_capacity(s.capacity()), alignment()));
  left_ = s.left_;

  value_type const* e = s.next_;
  for (value_type const* b = s.next_ - s.size(); b != e; ++b) {
    next_ = 1 + new (next_) value_type(*b);
  }
  *capacity_address() = s.capacity();
  return *this;
}

template <typename T>
stack<T>::~stack() requires(not std::is_trivially_destructible_v<value_type>) {
  if (next_) {
    value_type* e = next_;
    value_type* b = e - size();
    while (b != e) { (--e)->~value_type(); }
    ::operator delete(static_cast<void*>(b), alignment());
  }
}

template <typename T>
stack<T>::~stack() requires(std::is_trivially_destructible_v<value_type>) {
  if (next_) {
    ::operator delete(static_cast<void*>(next_ - size()), alignment());
  }
}

template <typename T>
void stack<T>::push(value_type const& value) {
  if (left_ == 0) { Reallocate(); }
  next_ = 1 + new (next_) value_type(value);
  --left_;
}

template <typename T>
void stack<T>::push(value_type&& value) {
  if (left_ == 0) { Reallocate(); }
  next_ = 1 + new (next_) value_type(static_cast<value_type&&>(value));
  --left_;
}

template <typename T>
template <typename... Args>
requires std::constructible_from<T, Args...>
decltype(auto) stack<T>::emplace(Args&&... args) {
  if (left_ == 0) { Reallocate(); }
  next_ = 1 + new (next_) value_type(std::forward<Args>(args)...);
  --left_;
  return *next_;
}

template <typename T>
void stack<T>::pop() {
  (--next_)->~value_type();
  ++left_;
}

template <typename T>
void stack<T>::swap(stack& other) {
  std::swap(next_, other.next_);
  std::swap(left_, other.left_);
}

}  // namespace nth

#endif  // NTH_CONTAINER_STACK_H
