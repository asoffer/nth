#ifndef NTH_DEBUG_TRACE_INTERNAL_CONCEPTS_H
#define NTH_DEBUG_TRACE_INTERNAL_CONCEPTS_H

namespace nth::debug::internal_trace {

template <typename T>
concept Traced = requires {
  typename T::NthInternalIsDebugTraced;
};

template <typename T>
concept Traceable = requires {
  typename T::NthInternalIsDebugTraceable;
};

}  // namespace nth::debug::internal_trace

#endif  // NTH_DEBUG_TRACE_INTERNAL_CONCEPTS_H
