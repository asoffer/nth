#ifndef NTH_IO_LOG_H
#define NTH_IO_LOG_H

#include <atomic>
#include <chrono>
#include <iostream>
#include <string_view>

#include "nth/io/format.h"
#include "nth/io/universal_print.h"
#include "nth/log/internal/entry.h"
#include "nth/log/internal/indicator.h"

namespace nth {

enum class LogSeverity : int8_t {
  Panic,
  Error,
  Warning,
  Info,
  Debug,
};

namespace internal_log {

inline constexpr LogSeverity Severity_Panic   = LogSeverity::Panic;
inline constexpr LogSeverity Severity_Error   = LogSeverity::Error;
inline constexpr LogSeverity Severity_Warning = LogSeverity::Warning;
inline constexpr LogSeverity Severity_Info    = LogSeverity::Info;
inline constexpr LogSeverity Severity_Debug   = LogSeverity::Debug;

}  // namespace internal_log

struct Logger {
  struct Printer {
    void write(std::string_view s) { std::cerr << s; }
    void write(char c) { std::cerr << c; }
  };
  Printer printer;
  using Formatter = nth::UniversalFormatter;
  Formatter formatter;

  void Complete() { nth::Format<"\n">(printer, formatter); }

  void LogKey(char const* file, int line, LogSeverity severity) {
    switch (severity) {
      case LogSeverity::Panic:
      case LogSeverity::Error:
        nth::Format<"\x1b[41m{}\x1b[0;31m {} {}:{}]\x1b[0m ">(
            printer, formatter, SeverityCharacter(severity),
            std::chrono::system_clock::now(), file, line);
        break;
      case LogSeverity::Warning:
        nth::Format<"\x1b[30;43m{}\x1b[0;33m {} {}:{}]\x1b[0m ">(
            printer, formatter, SeverityCharacter(severity),
            std::chrono::system_clock::now(), file, line);
        break;
      case LogSeverity::Info:
      case LogSeverity::Debug:
        nth::Format<"\x1b[44m{}\x1b[0;34m {} {}:{}]\x1b[0m ">(
            printer, formatter, SeverityCharacter(severity),
            std::chrono::system_clock::now(), file, line);
    }
  }

 private:
  static char SeverityCharacter(LogSeverity severity) {
    return "PEWID"[static_cast<std::underlying_type_t<LogSeverity>>(severity)];
  }
};

namespace internal_log {

template <nth::FormatString Fmt>
struct DoFormat {
  explicit constexpr DoFormat(char const* file, int line, LogSeverity severity)
      : file_(file), line_(line), severity_(severity) {}

  template <typename... Ts>
  void operator->*(Entry<Ts...> entry) {
    nth::Logger logger;
    logger.LogKey(file_, line_, severity_);
    std::apply(
        [&](std::same_as<void const*> auto... args) {
          nth::Format<Fmt>(logger.printer, logger.formatter,
                           (*static_cast<Ts const*>(args))...);
        },
        entry.entries_);
    logger.Complete();
  }

 private:
  char const* file_;
  int line_;
  LogSeverity severity_;
};

// inline constinit std::atomic<void (*)(std::string_view, int, LogSeverity)>
//     should_log = [](std::string_view, int, LogSeverity) { return true; };
// inline constinit std::atomic<int32_t> log_epoch = 0;

}  // namespace internal_log

// void SetShouldLog(void (*fn)(std::string_view, int, LogSeverity)) {
//   internal_log::should_log.store(fn, std::memory_order_acquire);
//   internal_log::log_epoch.fetch_add(2, std::memory_order_relaxed);
//   internal_log::Indicator *indicator;
//   while (true) {
//     indicator = internal_log::log_indicators.load(std::memory_order_relaxed);
//     if (indicator)
//
//   }
//
// }

}  // namespace nth

#define NTH_LOG(severity, format_string)                                       \
  switch (0)                                                                   \
  default:                                                                     \
    ([&] {                                                                     \
      static ::nth::internal_log::Indicator indicator;                         \
      return indicator.off();                                                  \
    }())                                                                       \
        ? (void)0                                                              \
        : ::nth::internal_log::DoFormat<format_string>(                        \
              __FILE__, __LINE__, ::nth::internal_log::Severity_##severity)    \
                  ->*::nth::internal_log::Entry

#endif  // NTH_IO_LOG_H
