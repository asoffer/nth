#ifndef NTH_BASE_CORE_H
#define NTH_BASE_CORE_H

namespace nth::internal_core {

template <typename T>
struct rvalue_ref {
  using type = T&&;
};

template <typename T>
struct rvalue_ref<T&> {
  using type = T&&;
};

}  // namespace nth::internal_core

// The macros `NTH_FWD` and `NTH_MOVE` behave identically to `std::forward` and
// `std::move`. However these function templates can have noticeable costs
// during compilation due to the function template instantiation costs.

#define NTH_FWD(e) (static_cast<decltype(e)&&>(e))

#define NTH_MOVE(e)                                                            \
  (static_cast<typename ::nth::internal_core::rvalue_ref<decltype(e)>::type>(e))

#endif  // NTH_BASE_CORE_H
