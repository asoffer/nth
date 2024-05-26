#ifndef NTH_META_CONCEPTS_CONVERTIBLE_H
#define NTH_META_CONCEPTS_CONVERTIBLE_H

namespace nth {

// A concept matching types that are explicitly convertible to `To`.
template <typename From, typename To>
concept explicitly_convertible_to = requires(From const& from) {
  static_cast<To>(from);
};

}  // namespace nth

#endif  // NTH_META_CONCEPTS_CONVERTIBLE_H
