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
  using value_type = T;

  stable_container();

  template <typename BeginIterator, typename EndIterator>
  stable_container(BeginIterator b, EndIterator e);
  stable_container(std::initializer_list<value_type> list);

  stable_container(stable_container const&) = delete;
  stable_container(stable_container&&);
  stable_container& operator=(stable_container const&) = delete;
  stable_container& operator=(stable_container&&);

  ~stable_container();

  struct entry_type;
  struct const_entry_type;
  struct iterator;
  struct const_iterator;

  [[nodiscard]] iterator begin();
  [[nodiscard]] iterator end();
  [[nodiscard]] const_iterator begin() const;
  [[nodiscard]] const_iterator end() const;
  [[nodiscard]] const_iterator cbegin() const;
  [[nodiscard]] const_iterator cend() const;

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;

  [[nodiscard]] entry_type entry(size_t index);
  [[nodiscard]] const_entry_type entry(size_t index) const;
  [[nodiscard]] const_entry_type centry(size_t index) const;
  [[nodiscard]] value_type& operator[](size_t index);
  [[nodiscard]] value_type const& operator[](size_t index) const;

  entry_type insert(value_type const& v);
  entry_type insert(value_type&& v);

  template <typename... Args>
  entry_type emplace(Args&&... vs) requires std::constructible_from<T, Args...>;

 private:
  static uint64_t Log2Floor(uint64_t n) {
    return n == 0 ? -1 : 63 ^ __builtin_clzll(n);
  }

  value_type* insertion_location();

  struct chunk {
    value_type* start;
    value_type* end;
    value_type* cap;

    size_t remaining() const { return cap - end; }
  };
  stack<chunk> chunks_;
};

template <typename T>
stable_container<T>::stable_container() {
  value_type* ptr = static_cast<value_type*>(::operator new(sizeof(T)));
  chunks_.push({ptr, ptr, ptr + 1});
}

template <typename T>
template <typename BeginIterator, typename EndIterator>
stable_container<T>::stable_container(BeginIterator b, EndIterator e)
    : stable_container() {
  for (; b != e; ++b) { insert(*b); }
}

template <typename T>
stable_container<T>::stable_container(std::initializer_list<value_type> list)
    : stable_container(list.begin(), list.end()) {}

template <typename T>
stable_container<T>::~stable_container() {
  while (not chunks_.empty()) {
    auto [start, end, cap] = chunks_.top();
    chunks_.pop();
    for (; start != end; --end) { (end - 1)->~value_type(); }
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
  return chunks_.size() == 1 and chunks_.top().end != chunks_.top().cap;
}

template <typename T>
stable_container<T>::entry_type stable_container<T>::insert(value_type const& v) {
  size_t index = size();
  return entry_type(new (insertion_location()) value_type(v), index);
}

template <typename T>
stable_container<T>::entry_type stable_container<T>::insert(value_type&& v) {
  size_t index = size();
  return entry_type(new (insertion_location())
                   value_type(static_cast<value_type&&>(v)), index);
}

template <typename T>
template <typename... Args>
stable_container<T>::entry_type stable_container<T>::emplace(
    Args&&... args) requires std::constructible_from<T, Args...> {
  size_t index = size();
  return entry_type(new (insertion_location())
                   value_type(static_cast<Args&&>(args)...),
               index);
}

template <typename T>
stable_container<T>::value_type* stable_container<T>::insertion_location() {
  auto& [start, end, cap] = chunks_.top();
  if (end != cap) [[likely]] { return end++; }

  size_t size = chunks_.size();
  value_type* ptr =
      static_cast<value_type*>(::operator new(sizeof(T) * (1 << size)));
  chunks_.push({ptr, ptr, ptr + (1 << size)});
  return chunks_.top().end++;
}

template <typename T>
struct stable_container<T>::entry_type {
  value_type& operator*() const { return *ptr_; }
  value_type* operator->() const { return ptr_; }

  size_t index() const { return index_; }

 private:
  friend stable_container;
  entry_type(value_type* ptr, size_t index) : ptr_(ptr), index_(index) {}
  value_type* ptr_;
  size_t index_;
};

template <typename T>
struct stable_container<T>::const_entry_type {
  value_type const& operator*() const { return *ptr_; }
  value_type const* operator->() const { return ptr_; }

  size_t index() const { return index_; }

 private:
  friend stable_container;
  const_entry_type(value_type const* ptr, size_t index)
      : ptr_(ptr), index_(index) {}
  value_type const* ptr_;
  size_t index_;
};

template <typename T>
stable_container<T>::entry_type stable_container<T>::entry(size_t index) {
  // TODO: This is a particularly slow implementation.
  auto iter = begin();
  for (size_t i = 0; i < index; ++i) { ++iter; }
  return entry_type(std::addressof(*iter), index);
}

template <typename T>
stable_container<T>::const_entry_type stable_container<T>::entry(
    size_t index) const {
  // TODO: This is a particularly slow implementation.
  auto iter = begin();
  for (size_t i = 0; i < index; ++i) { ++iter; }
  return const_entry_type(std::addressof(*iter), index);
}

template <typename T>
stable_container<T>::const_entry_type stable_container<T>::centry(
    size_t index) const {
  // TODO: This is a particularly slow implementation.
  auto iter = begin();
  for (size_t i = 0; i < index; ++i) { ++iter; }
  return const_entry_type(std::addressof(*iter), index);
}

template <typename T>
stable_container<T>::value_type& stable_container<T>::operator[](size_t index) {
  return *entry(index);
}
template <typename T>
stable_container<T>::value_type const& stable_container<T>::operator[](
    size_t index) const {
  return *centry(index);
}

template <typename T>
struct stable_container<T>::iterator {
  iterator& operator++() {
    if (++ptr_ == stack_iter_->end and ++stack_iter_ != end_) {
      ptr_ = stack_iter_->start;
    }
    return *this;
  }

  iterator operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
  }

  [[nodiscard]] friend bool operator==(iterator const& lhs,
                                       iterator const& rhs) {
    return lhs.ptr_ == rhs.ptr_ and lhs.stack_iter_ == rhs.stack_iter_;
  }
  [[nodiscard]] friend bool operator!=(iterator const& lhs,
                                       iterator const& rhs) {
    return not(lhs == rhs);
  }
  [[nodiscard]] value_type& operator*() const { return *ptr_; }
  [[nodiscard]] value_type* operator->() const { return ptr_; }

  [[nodiscard]] operator const_iterator() const {
    return const_iterator(ptr_, stack_iter_, end_);
  }

 private:
  friend stable_container;

  explicit iterator(value_type* ptr, chunk* stack_iter, chunk* end)
      : ptr_(ptr), stack_iter_(stack_iter), end_(end) {}

  value_type* ptr_;
  chunk* stack_iter_;
  chunk* end_;
};

template <typename T>
struct stable_container<T>::const_iterator {
  const_iterator& operator++() {
    if (++ptr_ == stack_iter_->end and ++stack_iter_ != end_) {
      ptr_ = stack_iter_->start;
    }
    return *this;
  }

  const_iterator operator++(int) {
    auto copy = *this;
    ++*this;
    return copy;
  }

  [[nodiscard]] friend bool operator==(const_iterator const& lhs,
                                       const_iterator const& rhs) {
    return lhs.ptr_ == rhs.ptr_ and lhs.stack_iter_ == rhs.stack_iter_;
  }
  [[nodiscard]] friend bool operator!=(const_iterator const& lhs,
                                       const_iterator const& rhs) {
    return not(lhs == rhs);
  }
  [[nodiscard]] value_type const& operator*() const { return *ptr_; }
  [[nodiscard]] value_type const* operator->() const { return ptr_; }

 private:
  friend stable_container;
  friend iterator;

  explicit const_iterator(value_type const* ptr, chunk const* stack_iter,
                          chunk const* end)
      : ptr_(ptr), stack_iter_(stack_iter), end_(end) {}

  value_type const* ptr_;
  chunk const* stack_iter_;
  chunk const* end_;
};

template <typename T>
stable_container<T>::iterator stable_container<T>::begin() {
  if (empty()) { return end(); }
  std::span span = chunks_.top_span(chunks_.size());
  return iterator(span[0].start, span.data(), span.data() + span.size());
}

template <typename T>
stable_container<T>::iterator stable_container<T>::end() {
  return iterator(chunks_.top().end, &chunks_.top() + 1, &chunks_.top() + 1);
}

template <typename T>
stable_container<T>::const_iterator stable_container<T>::begin() const {
  if (empty()) { return end(); }
  std::span span = chunks_.top_span(chunks_.size());
  return const_iterator(span[0].start, span.data(), span.data() + span.size());
}

template <typename T>
stable_container<T>::const_iterator stable_container<T>::end() const {
  return const_iterator(chunks_.top().end, &chunks_.top() + 1,
                        &chunks_.top() + 1);
}

template <typename T>
stable_container<T>::const_iterator stable_container<T>::cbegin() const {
  if (empty()) { return end(); }
  std::span span = chunks_.top_span(chunks_.size());
  return const_iterator(span[0].start, span.data(), span.data() + span.size());
}

template <typename T>
stable_container<T>::const_iterator stable_container<T>::cend() const {
  return const_iterator(chunks_.top().end, &chunks_.top() + 1,
                        &chunks_.top() + 1);
}

}  // namespace nth

#endif  // NTH_CONTAINER_STABLE_CONTAINER_H
