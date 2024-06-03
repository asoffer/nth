#include "nth/types/extend/extend.h"

#include "nth/test/test.h"

namespace nth {

struct S : nth::extend<S>::with<> {};

NTH_TEST("extend/no-extensions") {}

}  // namespace nth
