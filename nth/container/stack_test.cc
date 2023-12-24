#include "nth/container/stack.h"

#include <array>
#include <string>

#include "nth/test/test.h"
#include "nth/test/benchmark.h"

namespace nth {
namespace {

NTH_TEST("stack/basic/default", auto type) {
  stack<nth::type_t<type>> s;
  NTH_EXPECT(s.empty());
  NTH_EXPECT(s.size() == size_t{0});
}

NTH_TEST("stack/basic/push", auto type) {
  using T = nth::type_t<type>;
  stack<T> s;

  s.push(T{});
  NTH_EXPECT(not s.empty());
  NTH_EXPECT(s.size() == size_t{1});

  T t{};
  s.push(t);
  NTH_EXPECT(not s.empty());
  NTH_EXPECT(s.size() == size_t{2});
}

NTH_TEST("stack/basic/push-pop-many-reallocations", auto type) {
  using T = nth::type_t<type>;
  stack<T> s;
  for (size_t i = 0; i < 100; ++i) { s.push(T{}); }

  NTH_EXPECT(not s.empty());
  NTH_EXPECT(s.size() == size_t{100});

  for (size_t i = 0; i < 100; ++i) { s.pop(); }

  NTH_EXPECT(s.empty());
  NTH_EXPECT(s.size() == size_t{0});
}

NTH_TEST("stack/basic/push-pop", auto type) {
  using T = nth::type_t<type>;
  stack<T> s;

  s.push(T{});
  s.pop();
  NTH_EXPECT(s.empty());
  NTH_EXPECT(s.size() == size_t{0});

  T t{};
  s.push(t);
  s.push(t);
  s.pop();
  NTH_EXPECT(not s.empty());
  NTH_EXPECT(s.size() == size_t{1});
  s.pop();
  NTH_EXPECT(s.empty());
  NTH_EXPECT(s.size() == size_t{0});
}

NTH_TEST("stack/basic/copy", auto type) {
  using T = nth::type_t<type>;
  stack<T> s1, s2;
  s1.push({});
  s1.push({});
  s2 = s1;
  NTH_EXPECT(s1.size() == size_t{2});
  NTH_EXPECT(s2.size() == size_t{2});
  NTH_EXPECT(s2.size() == size_t{2});

  auto s3 = s2;
  NTH_EXPECT(s1.size() == size_t{2});
  NTH_EXPECT(s2.size() == size_t{2});
  NTH_EXPECT(s3.size() == size_t{2});

  s1.push({});
  NTH_EXPECT(s1.size() == size_t{3});
  NTH_EXPECT(s2.size() == size_t{2});
  NTH_EXPECT(s3.size() == size_t{2});

  s2.push({});
  NTH_EXPECT(s1.size() == size_t{3});
  NTH_EXPECT(s2.size() == size_t{3});
  NTH_EXPECT(s3.size() == size_t{2});
}

NTH_TEST("stack/basic/move", auto type) {
  using T = nth::type_t<type>;
  stack<T> s1, s2;
  s1.push({});
  s1.push({});
  s2 = std::move(s1);
  NTH_EXPECT(s2.size() == size_t{2});

  auto s3 = std::move(s2);
  NTH_EXPECT(s3.size() == size_t{2});
}

NTH_INVOKE_TEST("stack/basic/*") {
  co_yield nth::type<std::string>;
  co_yield nth::type<uint64_t>;
  co_yield nth::type<uint32_t>;
  co_yield nth::type<uint16_t>;
  co_yield nth::type<uint8_t>;
  co_yield nth::type<std::array<char, 3>>;
  co_yield nth::type<std::array<char, 11>>;
}

NTH_TEST("stack/top") {
  stack<std::string> s;

  s.push("hello");
  NTH_EXPECT(s.top() == "hello");
  NTH_EXPECT(static_cast<stack<std::string> const&>(s).top() == "hello");
  s.top() = "world";
  NTH_EXPECT(s.top() == "world");
  NTH_EXPECT(static_cast<stack<std::string> const&>(s).top() == "world");
}

NTH_TEST("stack/top-span") {
  stack<int> s           = {0, 1, 2, 3, 4};
  std::span<int, 2> span = s.top_span<2>();
  NTH_EXPECT(span[0] == 3);
  NTH_EXPECT(span[1] == 4);
  s.pop();
  span = s.top_span<2>();
  NTH_EXPECT(span[0] == 2);
  NTH_EXPECT(span[1] == 3);
}

NTH_TEST("stack/emplace") {
  stack<std::string> s;

  s.emplace("hello");
  NTH_EXPECT(s.top() == "hello");
  NTH_EXPECT(static_cast<stack<std::string> const&>(s).top() == "hello");
}

NTH_TEST("stack/destruction") {
  size_t n = 0;
  struct DtorCount {
    explicit DtorCount(size_t *n) : n_(n) {}
    ~DtorCount() { ++*n_; }
    size_t *n_;
  };

  {
    stack<DtorCount> s;
    NTH_EXPECT(n == size_t{0});
    s.emplace(&n);
    NTH_EXPECT(n == size_t{0});
  }
  NTH_EXPECT(n == size_t{1});

  {
    stack<DtorCount> s;
    for (size_t i = 0; i < 100; ++i) { s.emplace(&n); }
    n = 0;  // Reset counter so as to ignore updates due to reallocations;
  }
  NTH_EXPECT(n == size_t{100});
}

NTH_TEST("stack/size-unchanged-when/reconstitute", auto s) {
  size_t previous_size = s.size();
  auto [ptr, remaining] = std::move(s).release();
  s                     = decltype(s)::reconstitute_from(ptr, remaining);
  NTH_EXPECT(s.size() == previous_size);
}

NTH_TEST("stack/size-unchanged-when/reserve", auto s) {
  size_t previous_size = s.size();
  s.reserve(s.size() + 1);
  NTH_EXPECT(s.size() == previous_size);
}

NTH_INVOKE_TEST("stack/size-unchanged-when/*") {
  co_yield stack<int>{};
  co_yield stack<int>{3};
  co_yield stack<int>{0, 1, 2, 3};

  co_yield stack<bool>{};
  co_yield stack<bool>{true};
  co_yield stack<bool>{true, false, true};

  co_yield stack<std::string>{};
  co_yield stack<std::string>{"hello"};
  co_yield stack<std::string>{"hello", "world"};
}


NTH_TEST("stack/benchmark/push", auto t) {
  NTH_MEASURE() {
    nth::stack<nth::type_t<t>> stack;
    NTH_TIME("push") {
      nth::DoNotOptimize(stack);
      stack.push(nth::type_t<t>{});
      nth::DoNotOptimize(stack);
    }
  }
}

NTH_INVOKE_TEST("stack/benchmark/push") {
  co_yield nth::type<int>;
  co_yield nth::type<std::string>;
}

}  // namespace
}  // namespace nth
