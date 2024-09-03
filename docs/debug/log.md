# //nth/debug/log

## Overview 

This target defines a macro `NTH_LOG` used for debug logging, and a few free functions controlling
its behavior.

Logging is built on top of the [interpolation library](/format/interpolate). In its simplest usage,
one would provide an interpolation as an argument to the `NTH_LOG` macro whose placeholders will be
filled in with logging arguments.

```
NTH_LOG("The first three primes are {}, {}, and {}") <<= {2, 3, 5};
```

Log macros also can accept an optional first argument; a parethesized path-like string-literal which
we call the _verbosity path_. The verbosity path is simply an identifier that gives us a mechanism
to dynamically enable or disable log statements.

```
NTH_LOG(("my/verbosity/path"), "What roll's down {}, alone or in {}?")
    <<= {"stairs", "pairs"};
```

Sometimes log statements do not require any interpolaction arguments, in which case, one can simply
elide the trailing `<<= {}`, as in:

```
NTH_LOG(("my/verbosity/path"), "It's log! It's log! It's big, it's heavy, it's wood!");
```

## Verbosity

Logs can be enabled or disabled by speficying globs.

* __`nth::log_verbosity_off`__: Accepts a glob and disables all enabled log statements whose
  verbosity path matches the glob.

* __`nth::log_verbosity_on`__: Accepts a glob and enables all disabled log statements whose
  verbosity path matches the glob.

* __`nth::log_verbosity_if`__: Accepts a glob and enables all log statements whose verbosity path
  mtaches the glob and disables all others.

Dynamically changing the verbosity of log statements can be expensive, because it requires
matching a glob against every log statement in the binary. Changing verbosity should be rare.

## Log sinks

Log sinks are globally registered objects inheriting from `nth::log_sink` which process objects of
type `nth::log_entry`. Log sinks are registered via the `nth::register_log_sink` free function and
cannot be unregistered. When a log line is evaluated, if the log is enabled, it will construct a
`nth::log_entry` and pass it to every registered log sink.
