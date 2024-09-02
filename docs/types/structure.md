# `//nth/types:structure`

## Overview

This target provides an enum `nth::structure` whose fields describe classifications of types. This
classification can be used as a mechanism for traversing the structure of a type. Different
use-cases may have different opinions about the structure of types (e.g., one may wish to think of a
`std::string` as primitive, an object type, or a character-sequence depending on the context), so
there is no prescribed assignment of an `nth::structure` to any types.

## `nth::structure`

* __`primitive`:__ A tag associated with types that are considered "primitive," or atomic. This
  association may be context-dependent. While `std::string` is not a builtin primitive type in the
  C++ language, strings are considered primitive for languages like JSON or YAML.

* __`associative`:__ A tag associated with types that represent values keyed on other values.
  `std::map`, and `absl::flat_hash_map` are common examples of templates representing associative
  structures. Note that C++ texts often refer to `std::set` as an "associative container," but the
  intended meaning here does not include `std::set` as associative because it does not associate
  keys with values.

* __`variant`:__ A tag associated with types that hold exactly one value of a type not known
  statically. The set of possible types held by the object need not be restricted in any way. In
  other words, while `std::variant` is a classic example of such a type in C++, `std::any` would
  also be such a type.

* __`sequence`:__ A tag associated with types holding sequences of data. This tag does not make any
  claims about whether data may be repeated, the storage layout, computational costs (or even
  feasibility) of insertion, deletion, modification, etc. This tag represents a categorization of
  types for which data is associated to some number of consecutive nonnegative integers starting at
  0.

* __`object`:__ A tag associated with types that are "struct-like" in the sense that they are
  associative but the keys are fixed.

* __`entry`:__ A tag associated with elements in a sequnece. Structurally, a sequence contains an
  ordered collection of entries, each of which may have its own structure. This tag will never be
  directly associated with a type, but can be useful when traversing the sturcture of a sequence.

* __`key`:__ A tag associated with the key in an associative or object structure. This tag will
  never be directly associated with a type, but can be useful when traversing the structure of an
  associative or object type.

* __`value`:__ A tag associated with the value in an associative or object structure. This tag will
  never be directly associated with a type, but can be useful when traversing the structure of an
  associative or object type.

* __`unknown`:__ A default tag associated with types whose structure is unspecified via the
  mechanisms provided below and cannot be deduced.
