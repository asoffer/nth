#include "nth/debug/verbosity.h"

static_assert(nth::v.always(nth::source_location::current()));
static_assert(nth::v.when(true)(nth::source_location::current()));
static_assert(not nth::v.when(false)(nth::source_location::current()));
static_assert(not nth::v.never(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::harden or
              nth::v.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::harden or
              not nth::v.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::debug or
              nth::v.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::debug or
              nth::v.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::fast or
              not nth::v.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::fast or
              not nth::v.debug(nth::source_location::current()));

static_assert(nth::build_mode != nth::build::optimize or
              not nth::v.harden(nth::source_location::current()));
static_assert(nth::build_mode != nth::build::optimize or
              not nth::v.debug(nth::source_location::current()));

int main() { return 0; }
