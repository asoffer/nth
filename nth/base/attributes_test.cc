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

static_assert(Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(tailcall)), "true") or
              Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(tailcall)), "false"));
static_assert(NTH_SUPPORTS_ATTRIBUTE(tailcall) !=
              Equal(STRINGIFY(NTH_ATTRIBUTE(tailcall)), ""));

static_assert(Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(inline_always)), "true") or
              Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(inline_always)), "false"));
static_assert(NTH_SUPPORTS_ATTRIBUTE(inline_always) !=
              Equal(STRINGIFY(NTH_ATTRIBUTE(inline_always)), ""));

static_assert(Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(trivial_abi)), "true") or
              Equal(STRINGIFY(NTH_SUPPORTS_ATTRIBUTE(trivial_abi)), "false"));
static_assert(NTH_SUPPORTS_ATTRIBUTE(trivial_abi) !=
              Equal(STRINGIFY(NTH_ATTRIBUTE(trivial_abi)), ""));

}  // namespace
}  // namespace nth

int main() { return 0; }
