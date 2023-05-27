#include "nth/log/log.h"

#include "gtest/gtest.h"

namespace nth {
namespace {

TEST(A, B) {
  NTH_LOG(Debug, "{}, {}"){1, 2};
}

}  // namespace
}  // namespace nth
