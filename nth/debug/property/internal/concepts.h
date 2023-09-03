#ifndef NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H
#define NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H

namespace nth::internal_debug {

template <typename T>
concept PropertyType = requires {
  typename T::NthInternalIsDebugProperty;
};

template <typename T>
concept PropertyFormatterType = requires {
  typename T::NthInternalIsPropertyFormatter;
};



}  // namespace nth::internal_debug

#endif  //  NTH_DEBUG_PROPERTY_INTERNAL_CONCEPTS_H
