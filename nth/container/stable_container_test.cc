#include "nth/container/stable_container.h"

#include <array>
#include <string>

#include "nth/test/test.h"
#include "nth/test/benchmark.h"

namespace nth {
namespace {

NTH_TEST("stable_container/basic/default", auto type) {
  stable_container<nth::type_t<type>> c;
  NTH_EXPECT(c.empty());
  NTH_EXPECT(c.size() == size_t{0});
}

NTH_TEST("stable_container/basic/insert", auto type) {
  using T = nth::type_t<type>;
  stable_container<T> c;

  c.insert(T{});
  NTH_EXPECT(not c.empty());
  NTH_EXPECT(c.size() == size_t{1});

  T t{};
  c.insert(t);
  NTH_EXPECT(not c.empty());
  NTH_EXPECT(c.size() == size_t{2});
}

NTH_TEST("stable_container/basic/insertion-stability") {
  stable_container<int> c;
  c.insert(0);
  auto entry = c.insert(1000);
  for (size_t i = 0; i < 100; ++i) { c.insert(i); }

  NTH_EXPECT(not c.empty());
  NTH_EXPECT(c.size() == size_t{102});
  NTH_EXPECT(*entry == 1000);
}

NTH_TEST("stable_container/basic/move-stability") {
  stable_container<int> c;
  c.insert(0);
  auto entry = c.insert(1000);
  for (size_t i = 0; i < 100; ++i) { c.insert(i); }

  auto c2 = std::move(c);

  NTH_EXPECT(not c2.empty());
  NTH_EXPECT(c2.size() == size_t{102});
  NTH_EXPECT(*entry == 1000);
}

NTH_TEST("stable_container/basic/move", auto type) {
  using T = nth::type_t<type>;
  stack<T> s1, s2;
  s1.push({});
  s1.push({});
  s2 = std::move(s1);
  NTH_EXPECT(s2.size() == size_t{2});

  auto s3 = std::move(s2);
  NTH_EXPECT(s3.size() == size_t{2});
}

NTH_INVOKE_TEST("stable_container/basic/*") {
  co_yield nth::type<std::string>;
  co_yield nth::type<uint64_t>;
  co_yield nth::type<uint32_t>;
  co_yield nth::type<uint16_t>;
  co_yield nth::type<uint8_t>;
  co_yield nth::type<std::array<char, 3>>;
  co_yield nth::type<std::array<char, 11>>;
}

}  // namespace
}  // namespace nth
