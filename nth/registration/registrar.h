#ifndef NTH_REGISTRATION_REGISTRAR_H
#define NTH_REGISTRATION_REGISTRAR_H

#include <cstdint>

#include "absl/synchronization/mutex.h"
#include "nth/registration/internal/registry_node.h"

namespace nth {

// A `registrar` maintains a list of objects of type `T`. The type `T` must be
// trivially copyable. One can access the list of all registered items via the
// member function `registry`. The resulting object can be iterated over and is
// valid for the lifetime of the `registrar`, providing the list of all `T` that
// were registered when `registry` was invoked. Note that there is no mechanism
// for un-registering.
//
// A common use-case is to have a global
// `nth::indestructible<nth::registrar<T>>` used for registering hooks into
// global APIs that never need to be unregistered.
template <typename T>
requires(std::is_trivially_copyable_v<T> and
         sizeof(T) == sizeof(uintptr_t)) struct registrar {
  using value_type = T;
  struct range_type {
    struct const_iterator {
      using value_type = T;

      const_iterator& operator++();
      const_iterator operator++(int);

      value_type operator*() const;
      value_type const* operator->() const;

      friend bool operator==(const_iterator, const_iterator) = default;
      friend bool operator!=(const_iterator, const_iterator) = default;

      explicit const_iterator(uintptr_t const* ptr = nullptr) : ptr_(ptr) {}

     private:
      uintptr_t const* ptr_;
    };
    const_iterator begin() const { return begin_; }
    const_iterator end() const { return const_iterator(); }

    range_type(const_iterator begin) : begin_(begin) {}

   private:
    const_iterator begin_;
  };

  void insert(value_type v) {
    absl::MutexLock lock(&mutex_);
    if (head_->count() < 6) {
      head_->increment_count();
    } else {
      head_ = new internal_registration::registry_node(head_, 1);
    }
    *(reinterpret_cast<value_type*>(head_->data_) + head_->count()) = v;
  }

  range_type registry() const;

  ~registrar() {}

 private:
  static internal_registration::registry_node tail_;
  internal_registration::registry_node* head_ = &tail_;
  mutable absl::Mutex mutex_;
};

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))
    internal_registration::registry_node registrar<T>::tail_ {
  nullptr, 0
};

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))  //
    registrar<T>::range_type::const_iterator& registrar<
        T>::range_type::const_iterator::operator++() {
  --ptr_;
  if ((reinterpret_cast<uintptr_t>(ptr_) %
       alignof(internal_registration::registry_node)) == 0) {
    ptr_ = reinterpret_cast<internal_registration::registry_node const*>(ptr_)
               ->data_[0];
    if (ptr_) { ptr_ += 6; }
  }
  return *this;
}

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))  //
    registrar<T>::range_type::const_iterator
    registrar<T>::range_type::const_iterator::operator++(int) {
  const_iterator copy = *this;
  ++*this;
  return copy;
}

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))  //
    registrar<T>::range_type::const_iterator::value_type
    registrar<T>::range_type::const_iterator::operator*() const {
  return *reinterpret_cast<value_type const*>(ptr_);
}

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))  //
    registrar<T>::range_type::const_iterator::value_type
    const* registrar<T>::range_type::const_iterator::operator->() const {
  return reinterpret_cast<value_type const*>(ptr_);
}

template <typename T>
requires(std::is_trivially_copyable_v<T> and sizeof(T) == sizeof(uintptr_t))  //
    registrar<T>::range_type registrar<T>::registry()
const {
  uintptr_t const* ptr;
  {
    absl::MutexLock lock(&mutex_);
    internal_registration::registry_node const* node = head_;
    if (node->count() == 0) {
      ptr = nullptr;
    } else {
      ptr = reinterpret_cast<uintptr_t const*>(&node->data_[0]) + node->count();
    }
  }
  return range_type(typename range_type::const_iterator(ptr));
}

}  // namespace nth

#endif  // NTH_REGISTRATION_REGISTRAR_H
