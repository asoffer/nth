#ifndef NTH_DEBUG_LOG_CONFIGURATION_H
#define NTH_DEBUG_LOG_CONFIGURATION_H

#include <optional>

#include "nth/debug/source_location.h"

namespace nth {

struct log_configuration {
  explicit log_configuration() = default;

  log_configuration& source_location(struct source_location const& location) {
    location_ = location;
    return *this;
  }

  std::optional<struct source_location> source_location() const {
    return location_;
  }

 private:
  std::optional<struct source_location> location_ =
      nth::source_location::current();
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_CONFIGURATION_H
