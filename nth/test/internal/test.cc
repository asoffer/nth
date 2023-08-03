#include "nth/test/internal/test.h"

#include <span>
#include <vector>

namespace nth {
namespace {

auto& TestInvocationRegsistry() {
  static auto& invocations = *new std::vector<TestInvocation>;
  return invocations;
}

}  // namespace

void RegisterTestInvocation(std::string_view categorization,
                            std::function<void()> test) {
  static std::vector<std::function<void()>> invocations;
  TestInvocationRegsistry().emplace_back(categorization, std::move(test));
}
std::span<TestInvocation> RegisteredTests() {
  return TestInvocationRegsistry();
}

}  // namespace nth
