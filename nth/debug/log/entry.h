#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

#include "nth/debug/log/line.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"

namespace nth {

// A particular invocation of an `NTH_LOG` statement along with encoded
// arguments.
struct log_entry {
  struct builder {
    struct nth_write_result_type {
      explicit constexpr nth_write_result_type(size_t n) : written_(n) {}

      constexpr size_t written() const { return written_; }

     private:
      size_t written_;
    };

    builder(builder const&) = delete;

    explicit builder(log_entry& entry NTH_ATTRIBUTE(lifetimebound))
        : entry_(entry) {}

    nth_write_result_type write(std::span<std::byte const> bytes);

   private:
    log_entry& entry_;
  };

  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<log_entry>) {
    if constexpr (S.empty()) {
      return nth::trivial_formatter{};
    } else {
      return nth::interpolating_formatter<S>{};
    }
  }

  friend void NthFormat(nth::io::writer auto& w, auto&,
                        log_entry const& entry) {
    nth::io::write_text(w, entry.data_);
  }

  size_t id() const { return id_; }

  friend struct log_line;

  log_entry() = delete;
  explicit log_entry(size_t id);

 private:
  friend builder;

  size_t id_;
  std::string data_;
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
