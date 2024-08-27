#include "nth/io/writer/writer.h"

#include <cstddef>
#include <span>

#include "nth/meta/type.h"

namespace nth::io {
namespace {

struct not_a_writer {};

struct a_writer {
  struct nth_write_result_type {
    size_t written() const;
  };
  nth_write_result_type write(std::span<std::byte const>);
};

static_assert(not writer<not_a_writer>);
static_assert(writer<a_writer>);
static_assert(writer<minimal_writer>);
static_assert(nth::type<write_result<a_writer>> ==
              nth::type<typename a_writer::nth_write_result_type>);
static_assert(nth::type<write_result<minimal_writer>> ==
              nth::type<basic_write_result>);
static_assert(write_result_type<basic_write_result>);

}  // namespace
}  // namespace nth::io

int main() {}
