#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ENABLER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ENABLER_H

#include <atomic>
#include <string_view>

#include "nth/base/section.h"
#include "nth/debug/source_location.h"

namespace nth::internal_contracts {

struct enabler {
  consteval enabler(std::string_view mode_path,
                    source_location location = nth::source_location::current())
      : mode_path_(mode_path), enabled_(true), source_location_(location) {}

  [[nodiscard]] constexpr size_t id() const;

  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  [[nodiscard]] constexpr std::string_view mode_path() const {
    return mode_path_;
  }

  constexpr bool enabled() const {
    return enabled_.load(std::memory_order::relaxed);
  }

  constexpr void enable(bool b = true) {
    enabled_.store(b, std::memory_order::relaxed);
  }

  constexpr void disable() {
    enabled_.store(false, std::memory_order::relaxed);
  }

 private:
  std::string_view mode_path_;
  std::atomic<bool> enabled_;
  struct source_location source_location_;
};

}  // namespace nth::internal_contracts

NTH_DECLARE_SECTION(nth_contract, ::nth::internal_contracts::enabler);

namespace nth::internal_contracts {

constexpr inline size_t enabler::id() const {
  return this - nth::section<"nth_contract">.begin();
}

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ENABLER_H
