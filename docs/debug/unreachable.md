# //nth/debug:unreachable

This target defines the macro `NTH_UNREACHABLE`. Invocations of this macro are always bugs, as this
code path indicates that the programmer believed this code path could never be reached in a valid
program. In debug builds, invocation of `NTH_UNREACHABLE` will log an error message and then abort
execution. Users may also provide an interpolation string and inject extra information that will be
appended to the log message, as if `NTH_UNREACHABLE` were `NTH_LOG`. That is, one could write

```
switch (n) {
    case 0: return DoCase0();
    case 1: return DoCase1();
    case 2: return DoCase2();
    default: NTH_UNREACHABLE("We should never have n = {}") <<= {n};
}
```

In optimized builds, invocation of `NTH_UNREACHABLE` is undefined, allowing compilers to optimize
under the assumption that this code path is never reached.
