#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

#include "nth/configuration/log.h"
#include "nth/debug/log/line.h"
#include "nth/debug/log/sink.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/io/writer/writer.h"

namespace nth {

// A particular invocation of an `NTH_LOG` statement along with encoded
// arguments.
struct log_entry {
  struct builder {
    struct write_result_type {
      explicit constexpr write_result_type(size_t n) : written_(n) {}

      constexpr size_t written() const { return written_; }

     private:
      size_t written_;
    };

    builder(builder const&) = delete;

    explicit builder(log_entry& entry NTH_ATTRIBUTE(lifetimebound))
        : entry_(entry) {}

    write_result_type write(std::span<std::byte const> bytes);

   private:
    log_entry& entry_;
  };

  using nth_format_spec = nth::interpolation_spec;

  friend format_spec<log_entry> NthFormatSpec(interpolation_string_view s,
                                              type_tag<log_entry>) {
    return format_spec<log_entry>(s);
  }

  friend void NthFormat(io::forward_writer auto& w, format_spec<log_entry>,
                        log_entry const& entry) {
    w.write(nth::byte_range(entry.data_));
  }

  log_line_id id() const { return id_; }

  template <interpolation_string>
  friend struct log_line;

  log_entry() = delete;
  explicit log_entry(log_line_id id);

 private:
  friend builder;

  log_line_id id_;
  // Log data is stored structurally as a tree with all the data needed to
  // traverse it in a preorder traversal stored inline. Each entry is prefixed
  // with four bytes. The first two bytes indicate the the number of bytes in
  // the subtree rooted at that element (including all prefixes). The next two
  // bytes indicate the number of bytes in just that element (including the
  // length prefix). In this way, a leaf can be detected by determining that
  // the number of bytes in the subtree is two more than the number of bytes
  // in the element itself.
  std::string data_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
