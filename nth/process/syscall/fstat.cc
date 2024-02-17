#include "nth/process/syscall/fstat.h"

#include "nth/base/attributes.h"

namespace nth::sys {

NTH_ATTRIBUTE(weak)
int fstat(int fd, struct ::stat *buf) { return ::fstat(fd, buf); }

}  // namespace nth::sys
