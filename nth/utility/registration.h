#ifndef NTH_UTILITY_REGISTRATION_H
#define NTH_UTILITY_REGISTRATION_H

#include <type_traits>

#include "nth/base/macros.h"
#include "nth/utility/no_destructor.h"

namespace nth {
namespace internal_registration {

struct Registry {
  template <typename T>
  auto &get() const {
    return NthRegisterer(static_cast<T const *>(nullptr));
  }
};

}  // namespace internal_registration

// Global registry which can be used to access objects registered via the
// `NTH_DECLARE_REGISTRATION_POINT` and `NTH_REGISTER` macros.
inline constexpr internal_registration::Registry registry;

}  // namespace nth

// Accepts a single type that is base class for types to be registered. For any
// use of `NTH_DECLARE_REGISTRATION_POINT`, the program must contain exactly one
// use of the `NTH_REGISTER` macro with the same base class as its first
// type-argument. The second type-argument will be registered as the
// implementation for the virtual base class. Calls to
// `nth::registry.get<MyBaseClass>()` will return references to an eternal
// object that derives from `MyBaseClass`. The base class may be
// const-qualified, in which case the associated invocation to `NTH_REGISTER`
// must have the same qualification. In such situations, the reterned reference
// will be a const-reference.
#define NTH_DECLARE_REGISTRATION_POINT(...)                                    \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE;                                   \
  NTH_TYPE(0, __VA_ARGS__) & NthRegisterer(NTH_TYPE(0, __VA_ARGS__) const *);

// Accepts two type-arguments, a base class, and an implementation deriving from
// that base class. The base class must have been previously passed to
// `NTH_DECLARE_REGISTRATION_POINT`, and this must be the only invocation of
// `NTH_REGISTER` with that base class in the entire program. Calls to
// `nth::registry.get<MyBaseClass>()` will return references to an eternal,
// default-constructed object of the derived class (as a reference to the base
// class).
#define NTH_REGISTER(...)                                                      \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE;                                   \
  static_assert(std::is_base_of_v<__VA_ARGS__>);                               \
  NTH_TYPE(0, __VA_ARGS__) & NthRegisterer(NTH_TYPE(0, __VA_ARGS__) const *) { \
    static nth::NoDestructor<NTH_TYPE(1, __VA_ARGS__)> object;                 \
    return *object;                                                            \
  }

#endif  // NTH_UTILITY_REGISTRATION_H
