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

template <typename T > 
struct decay_impl {
  using type = T;
};
template <typename T>
struct decay_impl<T const> : decay_impl<T> {};

template <typename T>
struct decay_impl<T volatile> : decay_impl<T> {};

template <typename T>
struct decay_impl<T const volatile> : decay_impl<T> {};

template <typename T>
struct decay_impl<T &> : decay_impl<T> {};

template <typename R, typename... Args>
struct decay_impl<R(Args...)> : decay_impl<R (*)(Args...)> {};

template <typename T, int N>
struct decay_impl<T[N]> : decay_impl<T *> {};

}  // namespace internal_concepts

// Represents a hypothetical value of type `T` to be used with concepts and
// type-traits. No instantiation of this function template has a definition, so
// it can only be used in unevaluated contexts.
template <typename T>
T value();

// A concept matching `B` precisely.
template <typename A, typename B>
concept precisely = (internal_concepts::precisely_impl<A, B> and
                     internal_concepts::precisely_impl<B, A>);

// A concept matching any of the listed types.
template <typename T, typename... Ts>
concept any_of = (nth::precisely<T, Ts> or ...);

// A concept matching any enumeration type (scoped or unscoped).
template <typename T>
concept enumeration = __is_enum(T);

template <typename T>
using decayed = internal_concepts::decay_impl<T>::type;

// A concept matching any type that decays to `T`.
template <typename T, typename U>
concept decays_to = nth::precisely<nth::decayed<T>, U>;

}  // namespace nth

#endif  // NTH_META_CONCEPTS_CORE_H
