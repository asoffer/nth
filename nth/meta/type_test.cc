#include "nth/meta/type.h"

#include <optional>
#include <variant>
#include <sstream>
#include <string>

#include "gtest/gtest.h"

namespace {

TEST(Type, Comparison) {
  constexpr bool b1 = (nth::type<int> == nth::type<int>);
  EXPECT_TRUE(b1);
  constexpr bool b2 = (nth::type<int> == nth::type<bool>);
  EXPECT_FALSE(b2);
  constexpr bool b3 = (nth::type<bool> == nth::type<bool>);
  EXPECT_TRUE(b3);
  constexpr bool b4 = (nth::type<int> != nth::type<int>);
  EXPECT_FALSE(b4);
  constexpr bool b5 = (nth::type<int> != nth::type<bool>);
  EXPECT_TRUE(b5);
  constexpr bool b6 = (nth::type<bool> != nth::type<bool>);
  EXPECT_FALSE(b6);
}

TEST(Type, Decayed) {
  EXPECT_EQ(nth::type<int>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int&>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int&&>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int const>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int const&>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int const&&>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int volatile&>.decayed(), nth::type<int>);
  EXPECT_EQ(nth::type<int[3]>.decayed(), nth::type<int*>);
  EXPECT_EQ(nth::type<int(&)[3]>.decayed(), nth::type<int*>);
  EXPECT_EQ(nth::type<int(bool)>.decayed(), nth::type<int (*)(bool)>);
  EXPECT_EQ(nth::type<int(bool(&)[3])>.decayed(),
            nth::type<int (*)(bool(&)[3])>);
}

TEST(Type, TypeT) {
  EXPECT_EQ(nth::type<nth::type_t<nth::type<int>>>, nth::type<int>);
  EXPECT_EQ(nth::type<nth::type_t<nth::type<bool>>>, nth::type<bool>);
}

TEST(Type, Print) {
  auto to_string = [](nth::Type auto t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
  };

  EXPECT_EQ(to_string(nth::type<int>), "int");
  EXPECT_EQ(to_string(nth::type<int**>), "int**");
  EXPECT_EQ(to_string(nth::type<const int* const*>), "int const* const*");
}

TEST(Type, IsA) {
  constexpr bool b0 = nth::type<int>.is_a<std::optional>();
  EXPECT_FALSE(b0);

  constexpr bool b1 = nth::type<std::optional<int>>.is_a<std::optional>();
  EXPECT_TRUE(b1);

  constexpr bool b2 = nth::type<std::variant<int>>.is_a<std::optional>();
  EXPECT_FALSE(b2);

  constexpr bool b3 = nth::type<std::variant<int, bool>>.is_a<std::variant>();
  EXPECT_TRUE(b3);

  constexpr bool b4 = nth::type<std::optional<int>>.is_a<std::variant>();
  EXPECT_FALSE(b4);

  constexpr bool b5 = nth::type<std::string>.is_a<std::basic_string>();
  EXPECT_TRUE(b5);
}

TEST(Type, Address) {
  auto x = nth::type<int>;
  auto y = x;
  EXPECT_EQ(&x, &y);
}

}  // namespace
