#ifndef NTH_CONTAINER_STABLE_CONTAINER_H
#define NTH_CONTAINER_STABLE_CONTAINER_H

#include <concepts>
#include <memory>

#include "nth/container/stack.h"

namespace nth {

// A `stable_container<T>` is a container that can hold elements of type `T`, in
// such a way that all each object inserted is given a stable address in memory
// that will never change for as long as that object resides in the container.
// The address is guaranteed to be stable even if the container is moved. If two
// objects are inserted that compare equal, two insertions will occur and each
// object will be placed in a separate location. There are no guarantees on the
// ordering of elements or their relative addresses, or even that addresses are
// comparable. There is no mechanism for removing specific items from the
// container; one must destroy, clear, or assign over a container
template <typename T>
struct stable_container {
  stable_container();

  template <typename BeginIterator, typename EndIterator>
  stable_container(BeginIterator b, EndIterator e);

  stable_container(stable_container const&) = delete;
  stable_container(stable_container&&);
  stable_container& operator=(stable_container const&) = delete;
  stable_container& operator=(stable_container&&);

  ~stable_container();

  using value_type = T;

  struct entry {
    value_type& operator*() const { return *ptr_; }
    value_type* operator->() const { return ptr_; }

   private:
    friend stable_container;
    entry(value_type* ptr) : ptr_(ptr) {}
    value_type* ptr_;
  };

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;

  entry insert(value_type const& v);
  entry insert(value_type&& v);

  template <typename... Args>
  entry emplace(Args&&... vs) requires std::constructible_from<T, Args...>;

 private:
  value_type* insertion_location();

  struct pair {
    value_type* start;
    value_type* end;

    size_t remaining() const { return end - start; }
  };
  stack<pair> chunks_;
};

template <typename T>
stable_container<T>::stable_container() {
  value_type* ptr = static_cast<value_type*>(::operator new(sizeof(T)));
  chunks_.push({ptr, ptr + 1});
}

template <typename T>
template <typename BeginIterator, typename EndIterator>
stable_container<T>::stable_container(BeginIterator b, EndIterator e)
    : stable_container() {
  for (; b != e; ++b) { insert(*b); }
}

template <typename T>
stable_container<T>::~stable_container() {
  while (not chunks_.empty()) {
    auto [start, end] = chunks_.top();
    chunks_.pop();
    for (; start != end - (1 << chunks_.size()); --start) {
      (start - 1)->~value_type();
    }
    ::operator delete(start);
  }
}

template <typename T>
stable_container<T>::stable_container(stable_container&& c)
    : chunks_(std::exchange(c.chunks_, {})) {}

template <typename T>
size_t stable_container<T>::size() const {
  return (size_t{1} << chunks_.size()) - chunks_.top().remaining() - 1;
}

template <typename T>
bool stable_container<T>::empty() const {
  return chunks_.size() == 1 and chunks_.top().start != chunks_.top().end;
}

template <typename T>
stable_container<T>::entry stable_container<T>::insert(value_type const& v) {
  return entry(new (insertion_location()) value_type(v));
}

template <typename T>
stable_container<T>::entry stable_container<T>::insert(value_type&& v) {
  return entry(new (insertion_location())
                   value_type(static_cast<value_type&&>(v)));
}

template <typename T>
template <typename... Args>
stable_container<T>::entry stable_container<T>::emplace(
    Args&&... args) requires std::constructible_from<T, Args...> {
  return entry(new (insertion_location()) value_type(static_cast<Args&&>(args)...));
}

template <typename T>
stable_container<T>::value_type* stable_container<T>::insertion_location() {
  auto& [start, end] = chunks_.top();
  if (start != end) [[likely]] { return start++; }

  size_t size     = chunks_.size();
  value_type* ptr =
      static_cast<value_type*>(::operator new(sizeof(T) * (1 << size)));
  chunks_.push({ptr, ptr + (1 << size)});
  return chunks_.top().start++;
}

}  // namespace nth

#endif  // NTH_CONTAINER_STABLE_CONTAINER_H
