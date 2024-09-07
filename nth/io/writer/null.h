#ifndef NTH_IO_WRITER_NULL_H
#define NTH_IO_WRITER_NULL_H

#include <cstddef>
#include <span>

#include "nth/io/writer/writer.h"

namespace nth::io {

// Writes data by taking no action, and unconditionally reporting success.
struct null_writer {
  basic_write_result write(std::span<std::byte const> data) {
    return basic_write_result(data.size());
  }
};

}  // namespace nth::io

#endif  // NTH_IO_WRITER_NULL_H
