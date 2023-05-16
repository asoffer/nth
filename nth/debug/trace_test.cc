#include "nth/debug/trace.h"

#include <string>

#include "gtest/gtest.h"

#define NTH_GTEST_EXPECT(expr)                                                 \
  do {                                                                         \
    NTH_EXPECT(expr) else { ADD_FAILURE(); }                                   \
  } while (false)

#define NTH_GTEST_ASSERT(expr)                                                 \
  do {                                                                         \
    NTH_EXPECT(expr) else {                                                    \
      ADD_FAILURE();                                                           \
      return;                                                                  \
    }                                                                          \
  } while (false)

template <typename... Ts>
NTH_DECLARE_TRACE_API(std::vector<Ts...>, (data)(front)(back)(size)(operator[]));

namespace nth {
namespace {

TEST(Trace, Construction) {
  std::vector<double> vec{1, 2, 4};
  auto v = nth::Trace<"v">(vec);

  NTH_GTEST_EXPECT(&v[1] - 1 == v.data());
}

}  // namespace
}  // namespace nth
