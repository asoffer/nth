#ifndef NTH_LOG_INTERNAL_ENTRY_H
#define NTH_LOG_INTERNAL_ENTRY_H

#include <array>
#include <memory>

#include "nth/strings/format.h"

namespace nth::internal_log {

template <typename... Ts>
struct Entry {
  explicit constexpr Entry(Ts const&... ts) : entries_{std::addressof(ts)...} {}

 private:
  template <nth::FormatString Fmt>
  friend struct DoFormat;

  std::array<void const*, sizeof...(Ts)> entries_;
};

template <typename... Ts>
Entry(Ts const&...) -> Entry<Ts...>;

}  // namespace nth::internal_log

#endif  // NTH_LOG_INTERNAL_ENTRY_H
