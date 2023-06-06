#include "nth/io/environment_variable.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

template <typename>
struct EnvironmentVariable : testing::Test {};

constexpr char const E[] = "NTH_IO_ENVIRONMENT_VARIABLE_TEST_KEY";
constexpr char const V[] = "NTH_IO_ENVIRONMENT_VARIABLE_TEST_VALUE";

constexpr auto e_types =
    nth::type_sequence<char const (&)[sizeof(E)], char const *,
                       std::string_view, std::string>;
constexpr auto v_types =
    nth::type_sequence<char const (&)[sizeof(V)], char const *,
                       std::string_view, std::string>;

using TypePairs =
    nth::type_t<e_types
                    .reduce([](auto... es) {
                      return (v_types.transform<[](auto v) {
                        return nth::type<std::pair<nth::type_t<decltype(es){}>,
                                                   nth::type_t<v>>>;
                      }>() + ...);
                    })
                    .reduce([](auto... ts) {
                      return nth::type<testing::Types<nth::type_t<ts>...>>;
                    })>;
TYPED_TEST_SUITE(EnvironmentVariable, TypePairs);

TYPED_TEST(EnvironmentVariable, LoadUnset) {
  using key_type = typename TypeParam::first_type;

  ::unsetenv(E);

  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  EXPECT_FALSE(result.has_value());
}

TYPED_TEST(EnvironmentVariable, LoadSet) {
  using key_type   = typename TypeParam::first_type;
  using value_type = typename TypeParam::second_type;

  ::setenv(E, V, 1);

  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, static_cast<value_type>(V));
}

TYPED_TEST(EnvironmentVariable, LoadSeesCallToStoreDirect) {
  using key_type   = typename TypeParam::first_type;
  using value_type = typename TypeParam::second_type;

  ::setenv(E, "value", 1);
  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, "value");

  StoreEnvironmentVariable(static_cast<key_type>(E),
                           static_cast<value_type>(V));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, V);

  StoreEnvironmentVariable(static_cast<key_type>(E), std::nullopt);
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_FALSE(result.has_value());
}

TYPED_TEST(EnvironmentVariable, LoadSeesCallToStoreThroughOptional) {
  using key_type   = typename TypeParam::first_type;
  using value_type = typename TypeParam::second_type;

  ::setenv(E, "value", 1);
  std::optional<std::string> result =
      LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, "value");

  StoreEnvironmentVariable(static_cast<key_type>(E),
                           std::optional(static_cast<value_type>(V)));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, V);
  StoreEnvironmentVariable(
      static_cast<key_type>(E),
      std::optional<std::decay_t<value_type>>(std::nullopt));
  result = LoadEnvironmentVariable(static_cast<key_type>(E));
  ASSERT_FALSE(result.has_value());
}

TYPED_TEST(EnvironmentVariable, EnvironmentVisibleToChildren) {
  using key_type   = typename TypeParam::first_type;
  using value_type = typename TypeParam::second_type;


  StoreEnvironmentVariable(static_cast<key_type>(E),
                           static_cast<value_type>(V));

  // Using death test as a mechanism for testing that the environment variables
  // are propogated to child processes.
  EXPECT_DEATH(
      {
        std::optional<std::string> result =
            LoadEnvironmentVariable(static_cast<key_type>(E));
        if (result == V) { std::abort(); }
      },
      "");
}

// TODO: Add tests that cover thread-safety.

}  // namespace
}  // namespace nth
