# `//nth/base:pack`

## Overview

A collection of macros that wrap parameter-pack expansion idioms. The core macro is `NTH_PACK` which
accepts an initial parethesized argument indicating what to do, followed by the pack itself.

## `each`

### Usage

> `NTH_PACK((each), <pack>)`

Evaluates each expression in the pack. Evaluation is guaranteed to happen in
pack-order.

### Example

```
void invoke_on_each(auto f, auto&&... args) {
  NTH_PACK((each), f(args));
}

// `invoke_on_each(f, 1, 2, 3)` will evaluate `f(1)`, then `f(2)`, then `f(3)`.
```

## `reverse_each`

### Usage

> `NTH_PACK((reverse_each), <pack>)`

Evaluates each expression in the pack. Evaluation is guaranteed to happen in
reverse pack-order.

### Example

```
void reverse_invoke_on_each(auto f, auto&&... args) {
  NTH_PACK((reverse_each), f(args));
}

// `reverse_invoke_on_each(f, 1, 2, 3)` will evaluate `f(3)`, then `f(2)`, then `f(1)`.
```

## `invoke`

### Usage

> `NTH_PACK((invoke, <fn>), <pack>)`

Applies `fn` to each member of `pack`. Function invocation is guaranteed to happen in pack-order.

### Example

```
void invoke_on_each(auto f, auto&&... args) {
  NTH_PACK((invoke, f), args);
}

// `invoke_on_each(f, 1, 2, 3)` will evaluate `f(1)`, then `f(2)`, then `f(3)`.
```

## `any`

### Usage

> `NTH_PACK((any, <predicate>), <pack>)`

Evaluates to `true` if any member of `pack` satisfies `predicate`, and false otherwise.

## `all`

### Usage

> `NTH_PACK((all, <predicate>), <pack>)`

Evaluates to `true` if every member of `pack` satisfies `predicate`, and false otherwise.

## `count_if`

### Usage

> `NTH_PACK((count_if, <predicate>), <pack>)`

Evaluates to a `size_t` equal to the number of members of `pack` that satisfy `predicate`.

### Example

```
size_t num_with_size(size_t n, auto&&... args) {
  return NTH_PACK((invoke, [n](size_t s) { return s == n; }), sizeof(args));
}
```
