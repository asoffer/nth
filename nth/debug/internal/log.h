#ifndef NTH_DEBUG_INTERNAL_LOG_H
#define NTH_DEBUG_INTERNAL_LOG_H

#include <array>
#include <functional>
#include <string_view>
#include <utility>

#include "nth/base/attributes.h"
#include "nth/base/macros.h"
#include "nth/configuration/verbosity.h"
#include "nth/debug/source_location.h"
#include "nth/io/file_printer.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate.h"

namespace nth::internal_log {

template <int N>
struct UnserializedLogEntry {
  template <typename... Ts>
  UnserializedLogEntry(NTH_ATTRIBUTE(lifetimebound) Ts const&... ts)  //
      requires(sizeof...(Ts) == N)
      : entries_{Erased(ts)...} {}

  auto const& entries() const { return entries_; };

 private:
  struct Erased {
    template <typename T>
    Erased(NTH_ATTRIBUTE(lifetimebound) T const& t)
        : object_(std::addressof(t)),
          log_([](bounded_string_printer& p, void const* t) {
            nth::config::default_formatter()(p, *reinterpret_cast<T const*>(t));
          }) {}

    friend void NthPrint(bounded_string_printer& p, Erased e) {
      e.log_(p, e.object_);
    }

    void const* object_;
    void (*log_)(bounded_string_printer& p, void const*);
  };

  std::array<Erased, N> entries_;
};

// Represents a particular invocation of a log message.
struct LogLineBase {
  explicit LogLineBase(source_location location = source_location::current());

 protected:
  source_location source_location_;
  size_t index_ = 0;
  LogLineBase const * next_ = nullptr;
  static std::atomic<LogLineBase const*> head_;
};

struct LogEntry {
  std::string& data() & { return data_; }

  void set_placeholder_count(size_t placeholders) {
    offsets_.reserve(1 + placeholders);
  }

  void demarcate() { offsets_.push_back(data_.size()); }

  struct const_iterator {
    const_iterator& operator++() {
      ++offset_;
      return *this;
    }
    const_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }

    std::string_view operator*() const {
      size_t start = *(offset_ - 1);
      size_t end   = *offset_;
      return std::string_view(s_ + start, end - start);
    }

    friend bool operator==(const_iterator const&,
                           const_iterator const&) = default;
    friend bool operator!=(const_iterator const&,
                           const_iterator const&) = default;

   private:
    friend LogEntry;

    explicit const_iterator(char const* s,
                            typename std::vector<size_t>::const_iterator o)
        : s_(s), offset_(o) {}

    char const* s_;
    typename std::vector<size_t>::const_iterator offset_;
  };

  const_iterator begin() const {
    return const_iterator(data_.data(), std::next(offsets_.begin()));
  }

  const_iterator end() const {
    return const_iterator(data_.data(), offsets_.end());
  }

 private:
  std::string data_;
  std::vector<size_t> offsets_ = {0};
};

inline std::array<file_printer*, 1> RegisteredLoggers() {
  return {&stderr_printer};
}

struct TypeSink {
  constexpr TypeSink() = default;
  TypeSink(LogLineBase const&) {}
  TypeSink(struct Ignorer const&) {}
  TypeSink(TypeSink const&) = delete;
  TypeSink(TypeSink&&)      = delete;

  void operator=(TypeSink const&) = delete;
  void operator=(TypeSink&&) {}
};

struct Ignorer {
  constexpr Ignorer()     = default;
  Ignorer(Ignorer const&) = delete;
  Ignorer(Ignorer&&)      = delete;

  void operator=(Ignorer const&) = delete;
  void operator=(Ignorer&&) {}
  TypeSink operator<<=(int) const { return TypeSink(); }
};
inline constexpr Ignorer ignorer;

template <InterpolationString I>
struct LogLine : LogLineBase {
  friend TypeSink operator<<=(
      LogLine const& l,
      NTH_ATTRIBUTE(lifetimebound)
          UnserializedLogEntry<I.placeholders()> const& entry) {
    LogEntry log_entry;
    log_entry.set_placeholder_count(I.placeholders());

    constexpr size_t bound = 1024;
    bounded_string_printer printer(log_entry.data(), bound);

    auto formatter = nth::config::default_formatter();
    std::apply(
        [&](auto... entries) {
          ((nth::Interpolate<"{}">(printer, formatter, entries),
            log_entry.demarcate()),
           ...);
        },
        entry.entries());

    for (auto& logger : RegisteredLoggers()) {
      nth::Interpolate<"\x1b[0;34m{} {}:{}]\x1b[0m ">(
          *logger, formatter, l.source_location_.file_name(),
          l.source_location_.function_name(), l.source_location_.line());

      nth::InterpolateErased<I>(*logger, log_entry.begin(), log_entry.end());

      logger->write("\n");
    }

    return TypeSink();
  }
};

}  // namespace nth::internal_log

#define NTH_DEBUG_INTERNAL_LOG(interpolation_string)                           \
  NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(                                       \
      (::nth::config::default_log_verbosity_requirement),                      \
      interpolation_string)

#define NTH_DEBUG_INTERNAL_LOG_WITH_VERBOSITY(verbosity, interpolation_string) \
  NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(                               \
      ::nth::internal_log::TypeSink() = ([&] {                                 \
        [[maybe_unused]] constexpr auto& v = ::nth::debug_verbosity;           \
        return not(verbosity)(::nth::source_location::current());              \
      }())                                                                     \
                                            ? ::nth::internal_log::TypeSink{}  \
                                            : [&]() -> decltype(auto) {        \
        constexpr ::nth::InterpolationString is{(interpolation_string)};       \
        static const ::nth::internal_log::LogLine<is> nth_log_line;            \
        if constexpr (is.placeholders() == 0) {                                \
          nth_log_line <<= {};                                                 \
          return (::nth::internal_log::ignorer);                               \
        } else {                                                               \
          return (nth_log_line);                                               \
        }                                                                      \
      }())

#endif  // NTH_DEBUG_INTERNAL_LOG_H
