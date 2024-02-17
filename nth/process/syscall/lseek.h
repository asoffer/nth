#ifndef NTH_PROCESS_SYSCALL_LSEEK_H
#define NTH_PROCESS_SYSCALL_LSEEK_H

#include <sys/types.h>

#include "nth/debug/fakeable_function.h"

namespace nth::sys {

NTH_FAKEABLE(off_t, lseek, (int, fd)(off_t, offset)(int, whence));

}  // namespace nth::sys

#endif  // NTH_PROCESS_SYSCALL_LSEEK_H
