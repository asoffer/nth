# `//nth/base:configuration`

Provides a queryable interface to some build configuration information.

## `NTH_RTTI_ENABLED`

 A macro which expands to `true` if runtime type information is available and to `false` otherwise.

## `NTH_BUILD_MODE`

A function-like macro that expands to either `true` or `false`, to indicate that a particular build
mode is specified. Build modes can be

* `optimize`: Indicates that runtime execution speed is the primary concern. Other potential
  priorities such as debuggability or safety are considered less imporant.

* `harden`: Indicates that the primary concern for the executable is runtime execution speed, but
  that we are willing to tolerate a small performance loss for particularly important safety
  features. Other priorities, such as executable size or debuggability are not considered
  priorities.

* `debug`: Indicates that the primary concern for this executable is debuggability, and that other
  potential priorities (for example executable size, or runtime execution speed) are considered less
  important.

* `fast`: Indicates that the primary concern for this executable is compilation time, and that other
  priorities such as runtime execution speed, debuggability, or safety, may be sacrificed in order
  to compile the executable faster.

A build mode may only be specified on the commandline by defining the `NTH_COMMANDLINE_BUILD_MODE`
macro to one of these identifiers. If no build mode is specified, the library will attempt to detect
an appropriate mode, but without any guarantees.

The chosen build mode will be encoded in the constexpr variable `nth::build_mode`, whose type is
given by

```
enum class build {
  optimize,
  harden,
  debug,
  fast,
};
```
