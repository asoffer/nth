#include "nth/meta/compile_time_string.h"

#include <string_view>

#include "gtest/gtest.h"

namespace nth {

TEST(CompileTimeString, Construction) {
  constexpr CompileTimeString s("abc");
  EXPECT_EQ(s.size(), 3);
  EXPECT_EQ(s.length(), 3);
  EXPECT_EQ(s, std::string_view("abc"));
}

template <CompileTimeString S>
struct Wrapper {
  static constexpr std::string_view view = S;
};

TEST(CompileTimeString, TemplateParameters) {
  EXPECT_EQ(Wrapper<"abc">::view, "abc");
}

}  // namespace nth
