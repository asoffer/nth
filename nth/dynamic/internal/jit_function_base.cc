#include "nth/dynamic/internal/jit_function_base.h"

#include <sys/mman.h>

#include <cstring>

namespace nth::internal_dynamic {

jit_function_base::jit_function_base(jit_function_base&& f)
    : ptr_(f.ptr_), size_(f.size_) {
  f.ptr_  = nullptr;
  f.size_ = 0;
}

jit_function_base::~jit_function_base() {
  if (ptr_) { ::munmap(ptr_, size_); }
}

jit_function_base& jit_function_base::operator=(jit_function_base&& f) {
  ptr_    = f.ptr_;
  f.ptr_  = nullptr;
  size_   = f.size_;
  f.size_ = 0;
  return *this;
}

jit_function_base::jit_function_base(std::span<std::byte const> code) {
  size_ = code.size();
  ptr_  = ::mmap(0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                 -1, 0);
  if (ptr_ == MAP_FAILED) {
    ptr_ = nullptr;
  } else {
    std::memcpy(ptr_, code.data(), size_);
    if (::mprotect(ptr_, size_, PROT_READ | PROT_EXEC) == -1) {
      ::munmap(ptr_, size_);
      ptr_ = nullptr;
    }
  }
}

}  // namespace nth::internal_dynamic
