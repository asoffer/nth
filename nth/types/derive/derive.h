#ifndef NTH_TYPES_DERIVE_DERIVE_H
#define NTH_TYPES_DERIVE_DERIVE_H

#include <concepts>

#include "nth/base/core.h"
#include "nth/types/derive/internal/derive.h"
#include "nth/types/reflect.h"

namespace nth {

struct extension {
  template <std::derived_from<extension>...>
  struct implies;
};

template <std::derived_from<extension>... Extensions>
struct extension::implies : extension, internal_derive::nonterminal {
  template <typename... Ts>
  using concatenated = void (*)(Ts..., Extensions *...);
};

// `nth::deriving` is a mechanism for mixing behavior into a struct/class. To
// use extensions with a type `MyType`, it must inherit publicly from
// `nth::deriving<Extensions...>` where `Extensions` is a pack of extensions
// whose behavior should be mixed in to `MyType`.
template <typename... Extensions>
struct deriving
    : internal_derive::deriving_impl<decltype(internal_derive::all_extensions(
          static_cast<void (*)(Extensions *...)>(nullptr),
          static_cast<void (*)()>(nullptr)))> {
 private:
  static constexpr bool IsExtension =
      (std::derived_from<Extensions, ::nth::extension> and ...);
  // clang-format off
    static_assert(IsExtension, R"(
    Not all base classes of this type are \"extensions\". All base classes must
    inherit publicly from `nth::extension` or `nth::extension::implies<...>`.
    )");
  // clang-format on
};

}  // namespace nth

#endif  // NTH_TYPES_DERIVE_DERIVE_H
