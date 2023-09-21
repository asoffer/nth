#include "nth/test/test.h"

#include <span>
#include <vector>

namespace nth::test {
namespace {

auto& TestInvocationRegsistry() {
  static auto& invocations = *new std::vector<TestInvocation>;
  return invocations;
}

}  // namespace

void RegisterTestInvocation(std::string_view categorization,
                            std::function<void()> test) {
  TestInvocationRegsistry().emplace_back(categorization, std::move(test));
}

std::span<TestInvocation const> RegisteredTests() {
  return TestInvocationRegsistry();
}

}  // namespace nth::test
