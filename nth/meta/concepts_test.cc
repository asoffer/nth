#include "nth/meta/concepts.h"

#include "gtest/gtest.h"

namespace nth {

TEST(Concept, AnyOf) {
  {
    constexpr bool b = any_of<int, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = any_of<int, bool>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = any_of<int>;
    EXPECT_FALSE(b);
  }
  {
    constexpr bool b = any_of<int, bool, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = any_of<int, int, int>;
    EXPECT_TRUE(b);
  }
  {
    constexpr bool b = any_of<int, bool, char>;
    EXPECT_FALSE(b);
  }
}

TEST(Concept, NotAHasher) {
  struct NotAHasher {};
  constexpr bool i = hasher<NotAHasher, int>;
  EXPECT_FALSE(i);
}

TEST(Concept, HashesIntNotPointer) {
  struct HashesIntNotPointer {
    size_t operator()(int);
  };
  constexpr bool i = hasher<HashesIntNotPointer, int>;
  constexpr bool b = hasher<HashesIntNotPointer, bool>;
  constexpr bool p = hasher<HashesIntNotPointer, int*>;
  EXPECT_TRUE(i);
  EXPECT_TRUE(b);  // Implicit conversion from `bool` to `int`.
  EXPECT_FALSE(p);
}

TEST(Concept, BadReturnType) {
  struct HashesBadReturn {
    int* operator()(int);
  };
  constexpr bool i = hasher<HashesBadReturn, int>;
  constexpr bool p = hasher<HashesBadReturn, int*>;
  EXPECT_FALSE(i);
  EXPECT_FALSE(p);
}

TEST(Concept, ConvertingReturnType) {
  struct S {
    operator size_t();
  };
  struct HashesConverting {
    S operator()(int);
  };
  constexpr bool i = hasher<HashesConverting, int>;
  EXPECT_TRUE(i);
}

struct TemplateHasher {
  template <typename T>
  size_t operator()(T);
};

TEST(Concept, AcceptsTemplate) {
  constexpr bool i = hasher<TemplateHasher, int>;
  constexpr bool p = hasher<TemplateHasher, int*>;
  EXPECT_TRUE(i);
  EXPECT_TRUE(p);
}

}  // namespace nth
