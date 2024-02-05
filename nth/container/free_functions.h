#ifndef NTH_CONTAINER_FREE_FUNCTIONS_H
#define NTH_CONTAINER_FREE_FUNCTIONS_H

#include <utility>

namespace nth {

struct emplace_t {
  template <typename C, typename... Args>
  decltype(auto) operator()(C&& c, Args&&... args) const requires requires {
    std::forward<C>(c).emplace(std::forward<Args>(args)...);
  }
  { return std::forward<C>(c).emplace(std::forward<Args>(args)...); }

  template <typename C, typename... Args>
  decltype(auto) operator()(C&& c, Args&&... args) const requires requires {
    NthEmplace(std::forward<C>(c), std::forward<Args>(args)...);
  }
  { return NthEmplace(std::forward<C>(c), std::forward<Args>(args)...); }
};
inline constexpr emplace_t emplace;

struct reserve_t {
  template <typename C, typename... Args>
  decltype(auto) operator()(C&& c, Args&&... args) const requires requires {
    std::forward<C>(c).reserve(std::forward<Args>(args)...);
  }
  { return std::forward<C>(c).reserve(std::forward<Args>(args)...); }

  template <typename C, typename... Args>
  decltype(auto) operator()(C&& c, Args&&... args) const requires requires {
    NthReserve(std::forward<C>(c), std::forward<Args>(args)...);
  }
  { return NthReserve(std::forward<C>(c), std::forward<Args>(args)...); }
};
inline constexpr reserve_t reserve;

}  // namespace nth

#endif  // NTH_CONTAINER_FREE_FUNCTIONS_H
