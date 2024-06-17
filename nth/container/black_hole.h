#ifndef NTH_CONTAINER_BLACK_HOLE_H
#define NTH_CONTAINER_BLACK_HOLE_H

#include <cstddef>
#include <initializer_list>
#include <utility>

#include "nth/container/flyweight_map.h"
#include "nth/debug/debug.h"

namespace nth {

// A container that always reports as being empty.
template <typename T>
struct black_hole {
  using value_type = T;
  struct iterator {
    iterator &operator++() { NTH_UNREACHABLE(); }
    iterator operator++(int) { NTH_UNREACHABLE(); }

    iterator &operator--() { NTH_UNREACHABLE(); }
    iterator operator--(int) { NTH_UNREACHABLE(); }

    value_type &operator*() const { NTH_UNREACHABLE(); }
    value_type *operator->() const { NTH_UNREACHABLE(); }

    friend constexpr bool operator==(iterator, iterator) { return true; }
  };

  iterator insert(value_type const &) { return {}; }
  template <typename... Args>
  iterator emplace(Args &&...args) {
    static_cast<void>(T(std::forward<Args>(args)...));
    return {};
  }

  iterator begin() const { return {}; }
  iterator end() const { return {}; }
  bool empty() const { return true; }
  size_t size() const { return 0; }
};

}  // namespace nth

#endif  // NTH_CONTAINER_BLACK_HOLE_H
