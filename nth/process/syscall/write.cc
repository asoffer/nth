#include "nth/process/syscall/write.h"

#include <unistd.h>

#include "nth/base/attributes.h"

namespace nth::sys {

NTH_REAL_IMPLEMENTATION(ssize_t, write,
                        (int, fd)(void const*, ptr)(size_t, count)) {
  return ::write(fd, ptr, count);
}

}  // namespace nth::sys


