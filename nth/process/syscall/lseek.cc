#include "nth/process/syscall/lseek.h"

#include <unistd.h>

#include "nth/base/attributes.h"

namespace nth::sys {

NTH_REAL_IMPLEMENTATION(ssize_t, lseek, (int fd, off_t offset, int whence)) {
  return ::lseek(fd, offset, whence);
}

}  // namespace nth::sys

