#ifndef NTH_PROCESS_SYSCALL_READ_H
#define NTH_PROCESS_SYSCALL_READ_H

#include <sys/types.h>

#include "nth/test/fakeable_function.h"

namespace nth::sys {

NTH_FAKEABLE(ssize_t, read, (int fd, void *ptr, size_t count));

}  // namespace nth::sys

#endif  // NTH_PROCESS_SYSCALL_READ_H
