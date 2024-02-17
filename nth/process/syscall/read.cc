#include "nth/process/syscall/read.h"

#include <unistd.h>

#include "nth/base/attributes.h"

namespace nth::sys {

NTH_REAL_IMPLEMENTATION(ssize_t, read, (int fd, void* ptr, size_t count)) {
  return ::read(fd, ptr, count);
}

}  // namespace nth::sys
