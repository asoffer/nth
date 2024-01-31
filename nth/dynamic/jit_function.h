#ifndef NTH_DYNAMIC_JIT_FUNCTION_H
#define NTH_DYNAMIC_JIT_FUNCTION_H

#include <cstddef>
#include <span>

#include "nth/dynamic/internal/jit_function_base.h"

namespace nth {

template <typename>
struct jit_function;

template <typename Return, typename... Parameters>
struct jit_function<Return(Parameters...)>
    : private internal_dynamic::jit_function_base {
  using function_type = Return(Parameters...);

  // Constructs an empty (and therefore uninvocable) `jit_function`.
  explicit jit_function() = default;

  // Constructs a `jit_function` from the specified `code`. Care must be taken
  // to ensure `code` represents the contents of a compiled function which can
  // be invoked on the current architecture and properly accepts and returns the
  // given parameter and return types. Behavior is undefined if `code` does not
  // satisfy these constraints. The constructed function may be empty if the
  // system does not have the resources available to properly support the input.
  explicit jit_function(std::span<std::byte const> code)
      : internal_dynamic::jit_function_base(code) {}

  // Returns `true` if the function can be invoked and `false` otherwise.
  explicit operator bool() const { return ptr_; }

  // Returns the underlying function-pointer
  explicit operator function_type*() const {
    return reinterpret_cast<Return (*)(Parameters...)>(ptr_);
  }

  // Invokes the function.
  Return operator()(Parameters... parameters) {
    return reinterpret_cast<Return (*)(Parameters...)>(ptr_)(parameters...);
  }
};

}  // namespace nth

#endif  // NTH_DYNAMIC_JIT_FUNCTION_H
