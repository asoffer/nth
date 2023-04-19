#include "nth/coroutine/coroutine.h"

#include <string>

#include "gtest/gtest.h"
#include "nth/meta/type.h"
#include "nth/utility/buffer.h"

namespace nth {

TEST(Coroutine, Trivial) {
  auto trivial = []() -> coroutine<> { co_return; };
  trivial();
}

TEST(Coroutine, ReturningCoroutine) {
  auto returning = []() -> coroutine<int> { co_return 3; };
  auto c         = returning();
  EXPECT_EQ(c.result(), 3);
}

TEST(Coroutine, ReturningCoroutineWithInput) {
  auto returning = [](int n) -> coroutine<int> { co_return n* n; };
  auto c         = returning(5);
  EXPECT_EQ(c.result(), 25);
}

TEST(Coroutine, ReturningCoroutineWithNonTrivialType) {
  auto returning = [](std::string s) -> coroutine<std::string> {
    co_return s + s;
  };
  auto c         = returning("hello");
  EXPECT_EQ(c.result(), "hellohello");
}

TEST(Coroutine, WithMultipleInputs) {
  auto coro =
      []() -> coroutine<std::string, buffer_sufficient_for<int, std::string>> {
    int n = co_await type<int>;
    std::string result;
    for (int i = 0; i < n; ++i) { result += co_await type<std::string>; }
    co_return result;
  };
  auto c = coro();
  c.send<int>(3);
  c.send<std::string>("hello");
  c.send<std::string>(", world");
  c.send<std::string>("!");
  EXPECT_EQ(c.result(), "hello, world!");
}

TEST(Coroutine, Yielding) {
  auto coro = []() -> coroutine<void, buffer_sufficient_for<int>> {
    int a = 0, b = 1;
    while (true) {
      co_yield a;
      std::tie(a, b) = std::pair(b, a + b);
    }
  };
  auto c = coro();
  EXPECT_EQ(c.get<int>(), 0);
  EXPECT_EQ(c.get<int>(), 1);
  EXPECT_EQ(c.get<int>(), 1);
  EXPECT_EQ(c.get<int>(), 2);
  EXPECT_EQ(c.get<int>(), 3);
  EXPECT_EQ(c.get<int>(), 5);
  EXPECT_EQ(c.get<int>(), 8);
  EXPECT_EQ(c.get<int>(), 13);
  EXPECT_EQ(c.get<int>(), 21);
}

TEST(Coroutine, YieldingWithReturn) {
  auto coro = [](int n) -> coroutine<std::string, buffer_sufficient_for<size_t>> {
    std::string a = "a", b = "b";
    for (int i = 0; i < n; ++i) {
      co_yield a.size();
      std::tie(a, b) = std::pair(b, a + b);
    }
    co_return a;
  };
  auto c = coro(6);
  EXPECT_EQ(c.get<size_t>(), 1);
  EXPECT_EQ(c.get<size_t>(), 1);
  EXPECT_EQ(c.get<size_t>(), 2);
  EXPECT_EQ(c.get<size_t>(), 3);
  EXPECT_EQ(c.get<size_t>(), 5);
  EXPECT_EQ(c.get<size_t>(), 8);
  EXPECT_EQ(c.result(), "abbabbababbab");
}

TEST(Coroutine, MultipleInteractingCoroutines) {
  auto iota = []() -> coroutine<void, buffer_sufficient_for<int>> {
    int n = 1;
    while (true) {
      co_yield n;
      ++n;
    }
  };
  auto factorial = [&]() -> coroutine<int, buffer_sufficient_for<int>> {
    auto gen   = iota();
    int result = 1;
    for (int n = co_await nth::type<int>, i = 0; i < n; ++i) {
      result *= gen.get<int>();
    }
    co_return result;
  };

  auto f = factorial();
  f.send<int>(10);
  EXPECT_EQ(f.result(), 3628800);
}

}  // namespace nth