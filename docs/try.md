# `//nth/try`

A library for simplifying early-exits.

## Overview

The `NTH_TRY` macro evaluates its expression and either returns early, or continues transforming the
value.

Consider, for example, a pointer. `NTH_TRY(some_pointer)` will determine whether the `some_pointer`
is null. If so, it will return `nullptr`. Otherwise, it will evaluate to the reference
`*some_pointer`.

## Custom Exit Handlers

One may specify an _exit_handler_, as an initial parenthesized argument to `NTH_TRY`, as in
`NTH_TRY((my_handler), ComputeSomeValue())`. The argument `my_handler` must be an expression
satisfying the `nth::try_exit_handler<T>` concept, where `T` is the type of `ComputeSomeValue()`.
Specifically,

* It must have a member function accepting a `okay` member function accepting a `T` and returning a
`bool` to indicate whether the value should be further transformed (`true`) or returned (`false`).

* It must contain a `transform_value` function to be invoked when the value is okay.

* It must contain a `transform_return` function to be invoked when the value is not okay and must be
  returned.

Note that the expression `my_handler` may be evaluated multiple times during evaluation of
`NTH_TRY`, so if the handler is intended to be stateful, users must ensure their handler is an
`lvalue` or otherwise has a mechanism to share the state.

## Custom Default Exit Handlers

The expression `nth::default_try_exit_handler<T>()` evaluates to a default handler, to be used
whenever a custom exit handler is not specified as the first argument to `NTH_TRY`. There are
several builtins: 

* All pointers have a builtin handler which detects and returns early on null pointers, and otherwise
  evaluates by dereferencing the pointer (evaluating to a reference to the pointee).

* `std::optional<T>` has a builtin handler which detects and returns early on `std::nullopt`, and
  otherwise evaluates to a reference to the contained object.

Users may define their own custom default handlers by defining the [FTADLE](ftadle) hook
`NthDefaultTryExitHandler`. Users must define a function of this name, findable via ADL, accepting
an `nth::type_tag<T>`, and returning the default handler to be used. Note that this expression may
be invoked multiple times, so if a user wants the handler to be stateful, they must ensure the
function returns a reference to a global handler, or somehow otherwise share the state.

## `NTH_UNWRAP`

Much like `NTH_TRY`, but aborts in the event of an error, rather than returning.

## `nth::try_main`

A custom exit handler primarily for use inside `main`. On failure the returned value will be `1`,
indicating unsuccessful program termination. Otherwise the value is transformed according to
`nth::default_try_exit_handler<T>`.
