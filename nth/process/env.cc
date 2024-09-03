#include "nth/process/env.h"

#include <cstdlib>
#include <mutex>

namespace nth::env {

static constinit std::mutex set_env_mutex;

std::optional<std::string> load(null_terminated_string_view name) {
  std::unique_lock lock(set_env_mutex);
  char const *s = std::getenv(name.data());
  if (s) { return s; }
  return std::nullopt;
}

void store(null_terminated_string_view name,
           null_terminated_string_view value) {
  std::unique_lock lock(set_env_mutex);
  ::setenv(name.data(), value.data(), 1);
}

void store(null_terminated_string_view name, std::nullopt_t) {
  std::unique_lock lock(set_env_mutex);
  ::unsetenv(name.data());
}

void store(null_terminated_string_view name,
           std::optional<null_terminated_string_view> value) {
  value ? nth::env::store(name, *value) : nth::env::store(name, std::nullopt);
}

}  // namespace nth::env
