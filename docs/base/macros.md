# `//nth/base:macros`

## Overview

This header contains several common macro utilities useful as building blocs for other macro-based
APIs.

## `NTH_REQUIRE_EXPANSION_TO_PREFIX_SUBEXPRESSION(...)`

Expands the given expression argument in such a way that it can be used as a prefix subexpression,
but nowhere else.

## `NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE`
Expands to a statement that is invalid except within the global namespace.

## `NTH_STRINGIFY(...)`
Expands to the stringification of the argument. For example,

```
NTH_STRINGIFY(hello)  // "hello"

#file "file.h"
NTH_STRINGIFY(__FILE__)  // "file.h"
```

## `NTH_FIRST_ARGUMENT(...)`
Expands to the first argument passed to the macro.

## `NTH_SECOND_ARGUMENT(...)`
Expands to the first argument passed to the macro.

## `NTH_TAIL_ARGUMENTS(...)`
Expands to all arguments except the first.

# `NTH_CONCATENATE(a, b)`

Expands to its arguments except that the last token of the first argument and the first token of the
second argument are concatenated together.

## `NTH_INVOKE(fn_macro, ...)`
Invokes the first argument passed to the macro on the remaining arguments.

## `NTH_IDENTITY(arg)`
Expands to the arguments passed in.

## `NTH_NOT(arg)`
The macro argument must expand to either `true` or `false`. The entire macro will expand to `true`
if the argument expands to `false`, and `false` if the argument expands to `true`.

## `NTH_IF(condition, t, f)`
The argument bound to `condition` must expand to either `true` or `false`. If
it expands to `true`, then the entire macro expands to the argument bound to
`t`. Otherwise the entire macro expands to the argument bound to `f`.

## `NTH_IS_EMPTY(...)`
Expands to `true` if no variadic arguments are passed and to `false` otherwise.

## `NTH_IS_PARENTHESIZED(arg)`
Expands to `true` if the argument is entirely enclosed in parentheses, and to `false` otherwise.

## `NTH_IGNORE(...)`
Expands to nothing.

## `NTH_IGNORE_PARENTHESES(arg)`

Expands either to its argument, if it is not parenthesesized, or to it arguments with the outermost
parentheses removed, if it is surrounded by parentheses.
