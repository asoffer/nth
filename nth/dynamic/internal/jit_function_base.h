#ifndef NTH_DYNAMIC_INTERNAL_JIT_FUNCTION_BASE_H
#define NTH_DYNAMIC_INTERNAL_JIT_FUNCTION_BASE_H

#include <cstddef>
#include <span>

#include "nth/dynamic/internal/jit_function_base.h"

namespace nth::internal_dynamic {

struct jit_function_base {
  jit_function_base()                          = default;
  jit_function_base(jit_function_base const &) = delete;
  jit_function_base(jit_function_base &&f);
  jit_function_base &operator=(jit_function_base const &) = delete;
  jit_function_base &operator=(jit_function_base &&f);
  ~jit_function_base();

  jit_function_base(std::span<std::byte const> code);

 protected:
  void *ptr_   = nullptr;
  size_t size_ = 0;
};

}  // namespace nth::internal_dynamic

#endif  // NTH_DYNAMIC_INTERNAL_JIT_FUNCTION_BASE_H
