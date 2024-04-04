#ifndef NTH_META_CONCEPTS_CONSTRUCTIBLE_H
#define NTH_META_CONCEPTS_CONSTRUCTIBLE_H

namespace nth {

template <typename T>
concept default_constructible = requires {
  new T;
};

template <typename T, typename... Args>
concept constructible_from = requires(Args... args) {
  new T(args...);
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_CONSTRUCTIBLE_H
