#include "nth/io/environment_variable.h"

#include "nth/meta/type.h"
#include "nth/test/test.h"

namespace nth {
namespace {

constexpr char const E[] = "NTH_IO_ENVIRONMENT_VARIABLE_TEST_KEY";
constexpr char const V[] = "NTH_IO_ENVIRONMENT_VARIABLE_TEST_VALUE";

NTH_INVOKE_TEST("environment-variable/*") {
  co_yield nth::TestArguments{nth::type<std::string>, nth::type<std::string>};
  co_yield nth::TestArguments{nth::type<std::string>,
                              nth::type<std::string_view>};
  co_yield nth::TestArguments{nth::type<std::string>, nth::type<char const *>};
  co_yield nth::TestArguments{nth::type<std::string>,
                              nth::type<char const(&)[sizeof(V)]>};

  co_yield nth::TestArguments{nth::type<std::string_view>,
                              nth::type<std::string>};
  co_yield nth::TestArguments{nth::type<std::string_view>,
                              nth::type<std::string_view>};
  co_yield nth::TestArguments{nth::type<std::string_view>,
                              nth::type<char const *>};
  co_yield nth::TestArguments{nth::type<std::string_view>,
                              nth::type<char const(&)[sizeof(V)]>};

  co_yield nth::TestArguments{nth::type<char const *>, nth::type<std::string>};
  co_yield nth::TestArguments{nth::type<char const *>,
                              nth::type<std::string_view>};
  co_yield nth::TestArguments{nth::type<char const *>, nth::type<char const *>};
  co_yield nth::TestArguments{nth::type<char const *>,
                              nth::type<char const(&)[sizeof(V)]>};

  co_yield nth::TestArguments{nth::type<char const(&)[sizeof(E)]>,
                              nth::type<std::string>};
  co_yield nth::TestArguments{nth::type<char const(&)[sizeof(E)]>,
                              nth::type<std::string_view>};
  co_yield nth::TestArguments{nth::type<char const(&)[sizeof(E)]>,
                              nth::type<char const *>};
  co_yield nth::TestArguments{nth::type<char const(&)[sizeof(E)]>,
                              nth::type<char const(&)[sizeof(V)]>};
}

NTH_TEST("environment-variable/load-unset", auto key, auto) {
  using key_type = nth::type_t<key>;

  ::unsetenv(E);

  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_EXPECT(not result.has_value());
}

NTH_TEST("environment-variable/load-set", auto key, auto value) {
  using key_type   = nth::type_t<key>;
  using value_type = nth::type_t<value>;

  ::setenv(E, V, 1);

  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == static_cast<value_type>(V));
}

NTH_TEST("environment-variable/load-sees-call-to-store-direct", auto key,
         auto value) {
  using key_type   = nth::type_t<key>;
  using value_type = nth::type_t<value>;

  ::setenv(E, "value", 1);
  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == "value");

  StoreEnvironmentVariable(static_cast<key_type>(E),
                           static_cast<value_type>(V));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == V);

  StoreEnvironmentVariable(static_cast<key_type>(E), std::nullopt);
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_EXPECT(not result.has_value());
}

NTH_TEST("environment-variable/load-sees-call-to-store-through-optional",
         auto key, auto value) {
  using key_type   = nth::type_t<key>;
  using value_type = nth::type_t<value>;

  ::setenv(E, "value", 1);
  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == "value");

  StoreEnvironmentVariable(static_cast<key_type>(E),
                           std::optional(static_cast<value_type>(V)));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_ASSERT(result.has_value());
  NTH_EXPECT(*result == V);
  StoreEnvironmentVariable(
      static_cast<key_type>(E),
      std::optional<std::decay_t<value_type>>(std::nullopt));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  NTH_EXPECT(not result.has_value());
}

// TODO: Add tests that cover thread-safety.
// TODO: Test that environment variables are propogated to child processes.

}  // namespace
}  // namespace nth
