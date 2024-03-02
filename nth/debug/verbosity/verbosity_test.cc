#include "nth/debug/verbosity/verbosity.h"

namespace nth::debug::internal_verbosity {

static_assert(V::always(nth::source_location::current()));
static_assert(V::when(true)(nth::source_location::current()));
static_assert(not V::when(false)(nth::source_location::current()));
static_assert(not V::never(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::harden or
              V::harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::harden or
              not V::debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::debug or
              V::harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::debug or
              V::debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::fast or
              not V::harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::fast or
              not V::debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::optimize or
              not V::harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::optimize or
              not V::debug(nth::source_location::current()));

}  // namespace nth::debug::internal_verbosity

int main() { return 0; }
