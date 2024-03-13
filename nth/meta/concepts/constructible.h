#ifndef NTH_META_CONCEPTS_CONSTRUCTIBLE_H
#define NTH_META_CONCEPTS_CONSTRUCTIBLE_H

namespace nth {

template <typename T, typename... Args>
concept default_constructible = requires {
  new T;
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_CONSTRUCTIBLE_H
