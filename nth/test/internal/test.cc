#include "nth/test/internal/test.h"

#include <span>
#include <vector>

namespace nth {
namespace {

auto& TestInvocationRegsistry() {
  static auto& invocations = *new std::vector<std::function<void()>>;
  return invocations;
}

}  // namespace

void RegisterTestInvocation(std::function<void()> test) {
  static std::vector<std::function<void()>> invocations;
  TestInvocationRegsistry().push_back(std::move(test));
}
std::span<std::function<void()> const> RegisteredTests() {
  return TestInvocationRegsistry();
}

}  // namespace nth
