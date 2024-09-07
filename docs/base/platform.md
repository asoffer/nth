# `//nth/base:platform`

## Overview

Provides a queryable interface to some build configuration information.

## `NTH_ARCHITECTURE`

A function-like macro that expands to either `true` or `false` depending on whether the argument
represents the target architecture. Valid arguments are `arm64`, `x64`, and `unknown`

## `NTH_BUILD_FEATURE`

A function-like macro that expands to either `true` or `false` depending on whether the build
feature is enabled. Valid arguments are:

* `rtti`: Indicates whether or not run-time type information is available.

* `asan`: Indicates whether or not address-sanitizer is enabled.

* `tsan`: Indicates whether or not thread-sanitizer is enabled.

## `NTH_BUILD_MODE`

A function-like macro that expands to either `true` or `false` depending on the build mode. Valid
arguments are:

* `debug`: Indicates that the program is built in such a way as to optimize for debugability.

* `fastbuild`: Indicates that the program is built in such a way as to optimize for compilation
  speed.

* `harden`: Indicates that the program is built in such a way as to optimize mostly for runtime
  performance, but may accept a minory performance penalty to accommodate some safety checks.

* `optimize`: Indicates that the program is built in such a way as to optimize for runtime
  performance.

## `NTH_COMPILER`

A function-like macro that expands to either `true` or `false` depending on whether the argument
represents the compiler. Valid arguments are `clang`, `gcc`, `msvc`, or `unknown`.

## `NTH_EXECUTABLE_FORMAT`

A function-like macro that expands to either `true` or `false` depending on whether the argument
represents the target executable format. Valid arguments are `elf`, `macho`, or `unknown`.

## `NTH_OPERATING_SYSTEM`

A function-like macro that expands to either `true` or `false` depending on whether the argument
represents the target operating system. Valid arguments are `windows`, `apple`, `andriod`, `linux`,
or `unknown`.
