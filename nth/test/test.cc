#include "nth/test/test.h"

#include <vector>

namespace nth {
namespace {

std::vector<Test const *> all_tests;

}  // namespace

Test::Test() { all_tests.push_back(this); }

std::span<Test const *const> RegisteredTests() { return all_tests; }

}  // namespace nth
