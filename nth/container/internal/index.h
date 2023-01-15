#ifndef NTH_CONTAINER_INTERNAL_INDEX_H
#define NTH_CONTAINER_INTERNAL_INDEX_H

#include <cstddef>

namespace nth::internal_container {
// We use a wrapper struct so that transparent hashing can distinguish the
// difference between a `size_t` and an index which happens to be implemented as
// such referring to a value of type `size_t`.
struct Index {
  size_t value;
};
}  // namespace nth::internal_container

#endif  // NTH_CONTAINER_INTERNAL_INDEX_H
