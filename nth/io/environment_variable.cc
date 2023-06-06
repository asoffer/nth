#include "nth/io/environment_variable.h"

#include <mutex>

namespace nth {
namespace internal_environment_variable {

static constinit std::mutex set_env_mutex;

std::optional<std::string> Get(char const *name) {
  std::unique_lock lock(internal_environment_variable::set_env_mutex);
  char const *s = std::getenv(name);
  if (s) { return s; }
  return std::nullopt;
}

void Unset(char const *name) {
  std::unique_lock lock(internal_environment_variable::set_env_mutex);
  ::unsetenv(name);
}

void Set(char const *name, char const *value) {
  std::unique_lock lock(internal_environment_variable::set_env_mutex);
  ::setenv(name, value, 1);
}

}  // namespace internal_environment_variable

}  // namespace nth
