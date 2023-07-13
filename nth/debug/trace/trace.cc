#include <vector>

namespace nth {
// namespace {

std::vector<void (*)()> expectation_failure_handlers;

// }  // namespace

void RegisterExpectationFailure(void (*handler)()) {
  expectation_failure_handlers.push_back(handler);
}

}  // namespace nth
