#ifndef NTH_META_CONCEPTS_CORE_H
#define NTH_META_CONCEPTS_CORE_H

namespace nth {
namespace internal_concepts {

template <typename, typename>
struct equal {
  static constexpr bool value = false;
};
template <typename A>
struct equal<A, A> {
  static constexpr bool value = true;
};

template <typename A, typename B>
concept precisely_impl = equal<A, B>::value;

}  // namespace internal_concepts

template <typename A, typename B>
concept precisely = (internal_concepts::precisely_impl<A, B> and
                     internal_concepts::precisely_impl<B, A>);

}  // namespace nth

#endif  // NTH_META_CONCEPTS_CORE_H
