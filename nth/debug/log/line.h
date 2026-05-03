#ifndef NTH_DEBUG_LOG_LINE_H
#define NTH_DEBUG_LOG_LINE_H

#include <atomic>
#include <memory>
#include <string_view>

#include "nth/base/core.h"
#include "nth/base/section.h"
#include "nth/debug/source_location.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/meta/concepts/core.h"
#include "nth/meta/type.h"

namespace nth {

// A `log_line` represents a location in source where logging may occur (often
// due to `NTH_LOG`. Log lines must be constructed with the annotation
// `NTH_PLACE_IN_SECTION(nth_log_line)`
struct log_line {
  struct default_config {};

  struct default_reader {
    using nth_log_config_type = ::nth::log_line::default_config;
    static std::shared_ptr<nth_log_config_type> parse(std::string_view) {
      return std::make_shared<nth_log_config_type>(default_config{});
    }
    static constexpr bool operator()(default_config) { return true; }
  };

  template <typename R>
  constexpr log_line(nth::type_tag<R>,
                     nth::source_location loc = nth::source_location::current())
      : source_location_(loc), parse_(log_line::parse<R>) {}
  template <typename R>
  constexpr log_line(std::string_view verbosity_path, nth::type_tag<R>,
                     nth::source_location loc = nth::source_location::current())
      : verbosity_path_(verbosity_path),
        source_location_(loc),
        parse_(log_line::parse<R>) {}

  template <nth::interpolation_string S>
  friend auto NthInterpolateFormatter(nth::type_tag<log_line>) {
    if constexpr (S.empty()) {
      return nth::trivial_formatter{};
    } else {
      return nth::interpolating_formatter<S>{};
    }
  }

  template <nth::interpolation_string S>
  friend void NthFormat(io::writer auto& w, nth::interpolating_formatter<S>&,
                        log_line const& line) {
    nth::interpolate<S>(w, line.source_location().file_name(),
                        line.source_location().line(),
                        line.source_location().function_name());
  }

  [[nodiscard]] constexpr size_t id() const;

  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  [[nodiscard]] constexpr std::string_view verbosity_path() const {
    return verbosity_path_;
  }

  // If a config is stored, invokes `reader` with the stored config and returns
  // the resulting `bool`. Otherwise, returns `false`.
  template <typename R>
  [[nodiscard]] bool enabled(R&& reader) const
    requires(requires {
      {
        NTH_FWD(reader)(
            std::declval<typename std::decay_t<R>::nth_log_config_type>())
      } -> nth::precisely<bool>;
    })
  {
    using config_type = std::decay_t<R>::nth_log_config_type;
    if (not enabled_.load(std::memory_order::acquire)) { return false; }

    if constexpr (nth::type<config_type> == nth::type<default_config>) {
      return true;
    } else {
      auto config = get_config();

      if (not config) { return false; }
      return NTH_FWD(reader)(*static_cast<config_type const*>(config.get()));
    }
  }

  // Parses `condition`, stores it as the new log condition and enables the log.
  // If parsing fails, the `log_line` remains unchanged.
  void enable(std::string_view condition) {
    auto parsed = parse_(condition);
    if (not parsed) { return; }
    set_config(std::move(parsed));
    enabled_.store(true, std::memory_order::release);
  }

  constexpr void disable() {
    enabled_.store(false, std::memory_order::relaxed);
  }

 private:
  template <typename R>
  static std::shared_ptr<void const> parse(std::string_view input) {
    return std::static_pointer_cast<void const>(R::parse(input));
  }

  // Repeatedly attempts to change `lock_` from `false` to `true` with `acquire`
  // semantics, failing if `lock_` did not hold `false` previously.
  void acquire_spin_lock() const {
    bool locked = false;
    while (not lock_.compare_exchange_weak(
        locked, true, std::memory_order::acquire, std::memory_order::relaxed)) {
      locked = false;
    }
  }

  // Sets `lock_` to `false` with `release` semantics.
  void release_spin_lock() const {
    lock_.exchange(false, std::memory_order::release);
  }

  std::shared_ptr<void const> get_config() const {
    acquire_spin_lock();
    auto ptr = condition_;
    release_spin_lock();
    return ptr;
  }

  void set_config(std::shared_ptr<void const> ptr) {
    acquire_spin_lock();
    condition_ = std::move(ptr);
    release_spin_lock();
  }

  std::string_view verbosity_path_;
  struct source_location source_location_;
  std::shared_ptr<void const> (*parse_)(std::string_view);
  std::shared_ptr<void const> condition_;
  std::atomic<bool> enabled_      = true;
  mutable std::atomic<bool> lock_ = false;
};

static_assert(sizeof(log_line) == 72);

}  // namespace nth

NTH_DECLARE_SECTION(nth_log_line, ::nth::log_line);

namespace nth {

constexpr inline size_t log_line::id() const {
  return this - nth::section<"nth_log_line">.begin();
}

}  // namespace nth

#endif  // NTH_DEBUG_LOG_LINE_H
