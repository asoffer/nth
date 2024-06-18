#include "nth/io/writer/writer.h"

#include <cstddef>
#include <span>

namespace nth::io {
namespace {

struct not_a_forward_writer {};

struct a_forward_writer : implements_forward_writer<a_forward_writer> {
  struct write_result_type {
    size_t written() const;
  };
  write_result_type write(std::span<std::byte const>);
};

static_assert(not forward_writer<not_a_forward_writer>);
static_assert(forward_writer<a_forward_writer>);
static_assert(forward_writer<minimal_forward_writer>);

}  // namespace
}  // namespace nth::io

int main() {}
