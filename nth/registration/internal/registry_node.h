#ifndef NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H
#define NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H

#include <cstdint>

namespace nth::internal_registration {

struct alignas(8 * alignof(uintptr_t)) registry_node {
  explicit registry_node(registry_node* next, uintptr_t count) : count_(count) {
    data_[0] = reinterpret_cast<uintptr_t*>(next);
  }

  void increment_count() { ++count_; }
  uintptr_t count() const { return count_; }

  uintptr_t* data_[7];
  uintptr_t count_;
};
static_assert(sizeof(registry_node) == 8 * sizeof(uintptr_t));

}  // namespace nth::internal_registration

#endif // NTH_REGISTRATION_INTERNAL_REGISTRY_NODE_H
