#ifndef NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H
#define NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H

#include <cstdint>

#include "nth/registration/internal/registry_node.h"

namespace nth::internal_registry {

template <typename T>
requires(sizeof(T) == sizeof(uintptr_t)) struct registrar {

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

 protected:
  struct ends {
    registry_node tail{nullptr, 0};
    registry_node* head = &tail;
    absl::Mutex mutex;

    void insert(value_type v) {
      absl::MutexLock lock(&mutex);
      if (head->count() < 6) {
        head->increment_count();
      } else {
        head = new registry_node(head, 1);
      }
      *(reinterpret_cast<value_type*>(head->data_) + head->count()) = v;
    }
  };
};

}  // namespace nth::internal_registry

#endif  // NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H
