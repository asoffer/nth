#ifndef NTH_META_FUNCTION_TYPE_H
#define NTH_META_FUNCTION_TYPE_H

#include <concepts>

#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace nth {

namespace internal_meta {

template <typename>
struct FunctionTypeImpl;

template <typename Ret, typename... Args>
struct FunctionTypeImpl<Ret(Args...)> {
  static constexpr auto arguments() { return ::nth::type_sequence<Args...>; }
  static constexpr auto return_type() { return ::nth::type<Ret>; }
};

}  // namespace internal_meta

template <std::invocable I>
constexpr internal_meta::FunctionTypeImpl<I> function_type;

}  // namespace nth

#endif  // NTH_META_FUNCTION_TYPE_H
