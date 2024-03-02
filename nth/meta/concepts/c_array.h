#ifndef NTH_META_CONCEPTS_C_ARRAY_H
#define NTH_META_CONCEPTS_C_ARRAY_H

namespace nth {
namespace internal_concepts {

template <typename T>
struct c_array {
  static constexpr bool value = false;
};

template <typename T>
struct c_array<T[]> {
  static constexpr bool value = true;
};

template <typename T, int N>
struct c_array<T[N]> {
  static constexpr bool value = true;
};

}  // namespace internal_concepts

template <typename T>
concept c_array = internal_concepts::c_array<T>::value;

}  // namespace nth

#endif  // NTH_META_CONCEPTS_C_ARRAY_H
