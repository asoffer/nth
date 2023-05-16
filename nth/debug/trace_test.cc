#include "nth/debug/trace.h"

#include <string>

#include "gtest/gtest.h"

#define NTH_GTEST_EXPECT(...)                                                  \
  do {                                                                         \
    NTH_EXPECT(__VA_ARGS__) else { ADD_FAILURE(); }                            \
  } while (false)

#define NTH_GTEST_ASSERT(...)                                                  \
  do {                                                                         \
    NTH_EXPECT(__VA_ARGS__) else {                                             \
      ADD_FAILURE();                                                           \
      return;                                                                  \
    }                                                                          \
  } while (false)

template <typename... Ts>
NTH_DECLARE_TRACE_API(
    std::vector<Ts...>,
    (at)(back)(capacity)(data)(empty)(front)(max_size)(operator[])(size));

namespace nth {
namespace {

TEST(Trace, Construction) {
  std::vector<double> vec{1, 2, 4};
  auto v = nth::Trace<"v">(vec);

  NTH_GTEST_EXPECT(&v[1] - nth::Trace<"n">(1) == v.data());
  NTH_GTEST_EXPECT(3 * 2 - 1 == 4);
}

}  // namespace
}  // namespace nth
