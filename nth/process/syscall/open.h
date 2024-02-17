#ifndef NTH_PROCESS_SYSCALL_OPEN_H
#define NTH_PROCESS_SYSCALL_OPEN_H

#include <fcntl.h>

#include "nth/test/fakeable_function.h"

namespace nth::sys {

NTH_FAKEABLE(int, open, (char const *path, int flags));

}  // namespace nth::sys

#endif  // NTH_PROCESS_SYSCALL_OPEN_H
