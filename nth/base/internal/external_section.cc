#include "nth/base/section.h"

NTH_DECLARE_SECTION("multi-tu", int);

// For the purposes of this test, we cannot place this in an anonymous
// namespace, otherwise the linker would be free to throw it out. In general
// though, it is allowed to have section declarations in anonymous namespaces.
NTH_PLACE_IN_SECTION("multi-tu") int external = 10;
