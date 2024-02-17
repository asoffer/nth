#include "nth/process/syscall/open.h"

#include <fcntl.h>

#include "nth/base/attributes.h"

namespace nth::sys {

NTH_REAL_IMPLEMENTATION(int, open, (char const *path, int flags)) {
  return ::open(path, flags);
}

}  // namespace nth::sys

