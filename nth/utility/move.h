#ifndef NTH_UTILITY_MOVE_H
#define NTH_UTILITY_MOVE_H

namespace nth {
namespace internal_move {
template <typename T>
using identity = T;
}  // namespace internal_move

template <typename T>
T&& move(T& t) {
  return static_cast<T&&>(t);
}

template <typename T>
T&& move(internal_move::identity<T>&& t) {
  return static_cast<T&&>(t);
}

}  // namespace nth

#endif  // NTH_UTILITY_MOVE_H
