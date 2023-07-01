#include "nth/debug/verbosity.h"

static_assert(nth::debug_verbosity.always(nth::source_location::current()));
static_assert(nth::debug_verbosity.when(true)(nth::source_location::current()));
static_assert(
    not nth::debug_verbosity.when(false)(nth::source_location::current()));
static_assert(not nth::debug_verbosity.never(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::harden or
              nth::debug_verbosity.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::harden or
              not nth::debug_verbosity.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::debug or
              nth::debug_verbosity.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::debug or
              nth::debug_verbosity.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::fast or
              not nth::debug_verbosity.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::fast or
              not nth::debug_verbosity.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::optimize or
              not nth::debug_verbosity.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::optimize or
              not nth::debug_verbosity.debug(nth::source_location::current()));

int main() { return 0; }
