#include "nth/process/exit_code.h"

#include "nth/test/test.h"
#include "nth/utility/early_exit_test_cases.h"

namespace nth {
namespace {

NTH_INVOKE_TEST("nth/utility/early-exit") { co_yield type<exit_code>; }

NTH_TEST("exit_code/conversion-to-bool") {
  NTH_EXPECT(static_cast<bool>(exit_code::success) == true);
  NTH_EXPECT(static_cast<bool>(exit_code::generic_error) == false);
}

}  // namespace
}  // namespace nth
