#ifndef NTH_PROCESS_SYSCALL_WRITE_H
#define NTH_PROCESS_SYSCALL_WRITE_H

#include <sys/types.h>

#include "nth/debug/fakeable_function.h"

namespace nth::sys {

NTH_FAKEABLE(ssize_t, write, (int, fd)(void const*, ptr)(size_t, count));

}  // namespace nth::sys

#endif  // NTH_PROCESS_SYSCALL_WRITE_H
