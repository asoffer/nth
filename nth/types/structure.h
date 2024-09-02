#ifndef NTH_TYPES_STRUCTURE_H
#define NTH_TYPES_STRUCTURE_H

namespace nth {

// `structure` is an enum that provides a classification of data types.
enum class structure {
  // A tag associated with types that are considered "primitive," or atomic.
  // This association may be context-dependent. While `std::string` is not a
  // builtin primitive type in the C++ language, strings are considered
  // primitive for languages like JSON or YAML.
  primitive,

  // A tag associated with types that represent values keyed on other values.
  // `std::map`, and `absl::flat_hash_map` are common examples of templates
  // representing associative structures. Note that C++ texts often refer to
  // `std::set` as an "associative container," but the intended meaning here
  // does not include `std::set` as associative because it does not associate
  // keys with values.
  associative,

  // A tag associated with types that hold exactly one value of a type not known
  // statically. The set of possible types held by the object need not be
  // restricted in any way. In other words, while `std::variant` is a classic
  // example of such a type in C++, `std::any` would also be such a type.
  variant,

  // A tag associated with types holding sequences of data. This tag does not
  // make any claims about whether data may be repeated, the storage layout,
  // computational costs (or even feasibility) of insertion, deletion,
  // modification, etc. This tag represents a categorization of types for which
  // data is associated to some number of consecutive nonnegative integers
  // starting at 0.
  sequence,

  // TODO:
  entry,
  key,
  value,

  object,

  // A default tag associated with types whose structure is unspecified via the
  // mechanisms provided below and cannot be deduced.
  unknown,
};

}  // namespace nth

#endif  // NTH_TYPES_STRUCTURE_H
