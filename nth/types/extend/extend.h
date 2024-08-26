#ifndef NTH_TYPES_EXTEND_EXTEND_H
#define NTH_TYPES_EXTEND_EXTEND_H

#include <concepts>

#include "nth/base/core.h"
#include "nth/types/extend/internal/extend.h"
#include "nth/types/reflect.h"

namespace nth {

template <typename T, template <typename> typename... Extensions>
struct extension : internal_extend::nonterminal {
  template <typename... Ts>
  using concatenated = void (*)(Ts..., Extensions<T>*...);
};
template <typename T>
struct extension<T> {};

// `nth::extend` is a mechanism for mixing behavior into a struct/class. To
// use extensions with a type `MyType`, it must inherit publicly from
// `nth::extend<MyType>::with<Extensions...>` where `Extensions` is a pack of
// extensions whose behavior should be mixed in to `MyType`.
template <typename T>
struct extend final {
  template <template <typename> typename... Extensions>
  struct with : internal_extend::extend_impl<
                    T, decltype(internal_extend::all_extensions(
                           static_cast<void (*)(Extensions<T>*...)>(nullptr),
                           static_cast<void (*)()>(nullptr)))> {};
  extend()  = delete;
  ~extend() = delete;
  // TODO private:
  //  static constexpr bool IsExtension =
  //      (std::derived_from<Extensions, ::nth::extension> and ...);
  //  // clang-format off
  //    static_assert(IsExtension, R"(
  //    Not all base classes of this type are \"extensions\". All base classes
  //    must inherit publicly from `nth::extension` or
  //    `nth::extension::implies<...>`.
  //    )");
  //  // clang-format on
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_EXTEND_H
