#ifndef NTH_BASE_INTERNAL_GLOBAL_REGISTRY_H
#define NTH_BASE_INTERNAL_GLOBAL_REGISTRY_H

#include "absl/synchronization/mutex.h"
#include "nth/utility/no_destructor.h"

namespace nth::internal_base {

struct alignas(8 * alignof(uintptr_t)) RegistryNode {
  explicit RegistryNode(RegistryNode* next, uintptr_t count) : count_(count) {
    data_[0] = reinterpret_cast<uintptr_t*>(next);
  }

  void increment_count() { ++count_; }
  uintptr_t count() const { return count_; }

  uintptr_t* data_[7];
  uintptr_t count_;
};
static_assert(sizeof(RegistryNode) == 8 * sizeof(uintptr_t));

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t)) struct RegistrarImpl {
  using value_type = T;
  struct Range {
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

    Range(const_iterator begin) : begin_(begin) {}

   private:
    const_iterator begin_;
  };

 protected:
  struct Ends {
    RegistryNode tail{nullptr, 0};
    RegistryNode* head = &tail;
    absl::Mutex mutex;

    void insert(value_type v) {
      absl::MutexLock lock(&mutex);
      if (head->count() < 6) {
        head->increment_count();
      } else {
        head = new RegistryNode(head, 1);
      }
      *(reinterpret_cast<value_type*>(head->data_) + head->count()) = v;
    }
  };
};

template <typename, typename T>
struct Registrar : RegistrarImpl<T> {
  using value_type = RegistrarImpl<T>::value_type;
  using range_type = RegistrarImpl<T>::Range;

  static void Register(value_type v) { registry_->insert(v); }

  static range_type Registry();

 private:
  static NoDestructor<typename RegistrarImpl<T>::Ends> registry_;
};

template <typename Key, typename T>
Registrar<Key, T>::range_type Registrar<Key, T>::Registry() {
  uintptr_t const* ptr;
  {
    absl::MutexLock lock(&registry_->mutex);
    RegistryNode const* node = registry_->head;
    if (node->count() == 0) {
      ptr = nullptr;
    } else {
      ptr = reinterpret_cast<uintptr_t const*>(&node->data_[0]) + node->count();
    }
  }
  return range_type(typename range_type::const_iterator(ptr));
}

template <typename Key, typename T>
NoDestructor<typename RegistrarImpl<T>::Ends> Registrar<Key, T>::registry_;

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t))
    RegistrarImpl<T>::Range::const_iterator& RegistrarImpl<
        T>::Range::const_iterator::operator++() {
  --ptr_;
  if ((reinterpret_cast<uintptr_t>(ptr_) % alignof(RegistryNode)) == 0) {
    ptr_ = reinterpret_cast<RegistryNode const*>(ptr_)->data_[0];
    if (ptr_) { ptr_ += 6; }
  }
  return *this;
}

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t))
RegistrarImpl<T>::Range::const_iterator
RegistrarImpl<T>::Range::const_iterator::operator++(int) {
  const_iterator copy = *this;
  ++*this;
  return copy;
}

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t))
RegistrarImpl<T>::Range::const_iterator::value_type
RegistrarImpl<T>::Range::const_iterator::operator*() const {
  return *reinterpret_cast<value_type const*>(ptr_);
}

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t))
RegistrarImpl<T>::Range::const_iterator::value_type const*
RegistrarImpl<T>::Range::const_iterator::operator->() const {
  return reinterpret_cast<value_type const*>(ptr_);
}

}  // namespace nth::internal_base

#endif  // NTH_BASE_INTERNAL_GLOBAL_REGISTRY_H
