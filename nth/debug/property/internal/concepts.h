#ifndef NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H
#define NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H

namespace nth::debug::internal_property {

template <typename T>
concept PropertyType = requires {
  typename T::NthInternalIsDebugProperty;
};

template <typename T>
concept PropertyFormatterType = requires {
  typename T::NthInternalIsPropertyFormatter;
};

}  // namespace nth::debug::internal_property

#endif  //  NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H
