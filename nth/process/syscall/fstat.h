#ifndef NTH_PROCESS_SYSCALL_FSTAT_H
#define NTH_PROCESS_SYSCALL_FSTAT_H

#include <sys/stat.h>

namespace nth::sys {

int fstat(int fd, struct ::stat *buf);

}  // namespace nth::sys

#endif  // NTH_PROCESS_SYSCALL_FSTAT_H
