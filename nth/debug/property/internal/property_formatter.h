#ifndef NTH_DEBUG_PROPERTY_INTERNAL_PROPERTY_FORMATTER_H
#define NTH_DEBUG_PROPERTY_INTERNAL_PROPERTY_FORMATTER_H

#include <cstddef>
#include <deque>
#include <utility>

#include "nth/configuration/verbosity.h"
#include "nth/debug/property/internal/implementation.h"
#include "nth/io/string_printer.h"
#include "nth/strings/interpolate.h"

namespace nth::internal_debug {

template <typename F>
struct PropertyFormatter {
  using NthInternalIsPropertyFormatter = void;

  explicit constexpr PropertyFormatter(F& f) : formatter_(f) {}

  template <CompileTimeString Name, typename Fn, typename... Ts>
  void operator()(auto& printer, Property<Name, Fn, Ts...> const& m) {
    DrawPrefix(printer);
    printer.write(m.name());
    printer.write("\n");

    auto& [pos, len] = positions_.emplace_back(0, sizeof...(Ts));
    std::apply([&](auto&... args) { (((*this)(printer, args), ++pos), ...); },
               m.arguments());
    positions_.pop_back();
  }

  void operator()(auto& printer, auto const& value) {
    DrawPrefix(printer);
    formatter_(printer, value);
    printer.write("\n");
  }

 private:
  void DrawPrefix(auto& printer) {
    printer.write("    ");
    for (auto iter = positions_.begin();
         iter != positions_.end() and iter + 1 != positions_.end(); ++iter) {
      auto& [pos, len] = *iter;
      if (pos == len) {
        printer.write("   ");
      } else if (pos + 1 == len) {
        printer.write("\u2570\u2500 ");
        ++pos;
      } else {
        printer.write("\u2502  ");
      }
    }

    if (not positions_.empty()) {
      auto& [pos, len] = positions_.back();
      if (pos + 1 == len) {
        printer.write("\u2570\u2500 ");
        ++pos;
      } else if (pos == len) {
        printer.write("   ");
      } else {
        printer.write("\u251c\u2500 ");
      }
    }
  }

  F& formatter_;
  std::deque<std::pair<size_t, size_t>> positions_;
};

}  // namespace nth::internal_debug

#endif  // NTH_DEBUG_PROPERTY_INTERNAL_PROPERTY_FORMATTER_H
