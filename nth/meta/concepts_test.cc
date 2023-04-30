#include "nth/meta/concepts.h"

#include <tuple>

#include "gtest/gtest.h"

namespace nth {
namespace {

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

TEST(Concept, Enumeration) {
  enum E {};
  struct S {};
  enum class C {};
  constexpr bool e = enumeration<E>;
  constexpr bool s = enumeration<S>;
  constexpr bool c = enumeration<C>;
  EXPECT_TRUE(e);
  EXPECT_FALSE(s);
  EXPECT_TRUE(c);
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

TEST(Concept, DecaysTo) {
  constexpr bool ref    = decays_to<int&, int>;
  constexpr bool cvref  = decays_to<int const volatile&, int>;
  constexpr bool fn_ptr = decays_to<void(), void (*)()>;
  constexpr bool array  = decays_to<int[3], int*>;
  EXPECT_TRUE(ref);
  EXPECT_TRUE(cvref);
  EXPECT_TRUE(fn_ptr);
  EXPECT_TRUE(array);
}

struct TupleLike {
  int a, b, c;
};

struct DoesntSpecializeSize {
  int a, b, c;
};
struct MissesOneElementSpecialization {
  int a, b, c;
};

}  // namespace
}  // namespace nth

template <>
struct std::tuple_size<nth::TupleLike> : std::integral_constant<size_t, 3> {};

template <size_t N>
struct std::tuple_element<N, nth::TupleLike> {
  using type = int;
};

template <>
struct std::tuple_size<nth::MissesOneElementSpecialization>
    : std::integral_constant<size_t, 3> {};

template <>
struct std::tuple_element<0, nth::MissesOneElementSpecialization> {
  using type = int;
};

template <>
struct std::tuple_element<1, nth::MissesOneElementSpecialization> {
  using type = int;
};

template <size_t N>
struct std::tuple_element<N, nth::DoesntSpecializeSize> {
  using type = int;
};

namespace nth {
namespace {

TEST(Concept, TupleProtocol) {
  constexpr bool t0 = tuple_like<std::tuple<>>;
  EXPECT_TRUE(t0);

  constexpr bool t1 = tuple_like<std::tuple<int>>;
  EXPECT_TRUE(t1);

  constexpr bool t2 = tuple_like<std::tuple<int, bool>>;
  EXPECT_TRUE(t2);

  constexpr bool t = tuple_like<TupleLike>;
  EXPECT_TRUE(t);

  constexpr bool size = tuple_like<DoesntSpecializeSize>;
  EXPECT_FALSE(size);

  constexpr bool element = tuple_like<MissesOneElementSpecialization>;
  EXPECT_FALSE(element);
}

}  // namespace
}  // namespace nth
