#include "nth/process/exit_code.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("exit_code/conversion-to-bool") {
  NTH_EXPECT(static_cast<bool>(exit_code::success) == true);
  NTH_EXPECT(static_cast<bool>(exit_code::generic_error) == false);
}

}  // namespace
}  // namespace nth
