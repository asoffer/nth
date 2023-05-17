#include "nth/base/attributes.h"

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

namespace nth {
namespace {

constexpr bool Equal(char const *lhs, char const *rhs) {
  for (;; ++lhs, ++rhs) {
    if (*lhs != *rhs) { return false; }
    if (*lhs == '\0') { return true; }
  }
}

static_assert(Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(lifetimebound)), "true") or
              Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(lifetimebound)), "false"));
static_assert(NTH_SUPPORTS_ATTRIBUTE(lifetimebound) !=
              Equal(STRINGIFY(NTH_ATTRIBUTE(lifetimebound)), ""));

}  // namespace
}  // namespace nth

int main() { return 0; }
