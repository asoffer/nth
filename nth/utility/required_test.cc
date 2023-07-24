#include "nth/utility/required.h"

#include "nth/test/test.h"

namespace nth {
namespace {

struct S {
  int field = required;
};

NTH_TEST("Required/allows-field-to-be-populated") {
  NTH_EXPECT(S{3}.field == 3);
  NTH_EXPECT(S{.field = 3}.field == 3);
  // TODO: We need a non-compile test to verify that this does what we expect it
  // to do.
}

}  // namespace
}  // namespace nth
