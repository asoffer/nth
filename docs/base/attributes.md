# `//nth/base:attributes`

## Overview

Defines macros which wrap compiler-specific attributes and gives them a consistent API. One may
query whether an attribute is supported via `NTH_SUPPORTS_ATTRIBUTE(<attribute_name>)`, and use the
attribute via `NTH_ATTRIBUTE(<attribute_name>)`. A compilation error will be reported for any
unsupported attribute. One may also use `NTH_ATTRIBUTE_TRY(<attribute_name>)` which will expand to
the appropriate attribute if it is supported and nothing otherwise.

## Available attributes

Note that not all of these attributes are available on all platforms.

### `lifetimebound`
Defines an attribute consistent with the proposed wg21.link/p0936r0. When attached to a function
parameter, indicates that the return value may reference the object bound to the attributed function
parameter.

### `weak`
Defines an attribute which may be used to mark symbol declarations and definitions as 'weak'. All
unattributed symbols are implicitly declared strong. During linking, if any strong symbol of the
same name exists all weak symbols will be ignored. Beyond this rule, the one-definition rule still
applies: There must be either exactly one strong symbol definition, or no strong symbol definitions
and one weak symbol definition.

### `tailcall`
Defines an attribute indicating that the annotated return statement must be tail-call optimized. Use
of this attribute is intended for situations where tail-call optimization is a requirement for
correctness, rather than an optimization. As such, no fallback implementation is supported if the
compiler does not support it.

### `inline_always`
Defines an attribute indicating that the annotated function must be inlined by the compiler. Note
that this attribute refers to how the compiler generates executable code, not the `inline` attribute
in C++.

### `inline_never`
Defines an attribute indicating that the annotated function must not be inlined by the compiler.
Note that this attribute refers to how the compiler generates executable code, not the `inline`
attribute in C++.

### `trivial_abi`
Defines an attribute indicating that the annotated struct is trivial for the purposes of calls,
indicating that it can be passed in registers.
