#include "nth/process/env.h"

#include "nth/meta/type.h"
#include "nth/test/test.h"

namespace nth {
namespace {

constexpr null_terminated_string_view E(null_terminated_string_view::from_array,
                                        "NTH_PROCESS_ENV_TEST_KEY");
constexpr null_terminated_string_view V(null_terminated_string_view::from_array,
                                        "NTH_PROCESS_ENV_TEST_VALUE");

NTH_TEST("environment-variable/load-unset") {
  ::unsetenv(E.data());

  std::optional<std::string> result = env::load(E);
  NTH_EXPECT(not result.has_value());
}

NTH_TEST("environment-variable/load-set") {
  ::setenv(E.data(), V.data(), 1);

  std::optional<std::string> result = env::load(E);
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == V);
}

NTH_TEST("environment-variable/load-sees-call-to-store-direct") {
  ::setenv(E.data(), "value", 1);
  std::optional<std::string> result = env::load(E);
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == "value");

  env::store(E, V);
  result = env::load(E);
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == V);

  env::store(E, std::nullopt);
  result = env::load(E);
  NTH_EXPECT(not result.has_value());
}

NTH_TEST("environment-variable/load-sees-call-to-store-through-optional") {
  ::setenv(E.data(), "value", 1);
  std::optional<std::string> result = env::load(E);
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == "value");

  env::store(E, std::optional(V));
  result = env::load(E);
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == V);
  env::store(E, std::optional<std::string>(std::nullopt));
  result = env::load(E);
  NTH_EXPECT(not result.has_value());
}

// TODO: Add tests that cover thread-safety.
// TODO: Test that environment variables are propogated to child processes.

}  // namespace
}  // namespace nth
