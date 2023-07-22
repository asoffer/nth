#ifndef NTH_TEST_STD_TRACING_H
#define NTH_TEST_STD_TRACING_H

#include <span>

#include "nth/debug/trace/trace.h"

template <typename T, size_t N>
NTH_TRACE_DECLARE_API_TEMPLATE((std::span<T, N>),
                               (back)(begin)(data)(empty)(end)(first)         //
                               (front)(last)(rbegin)(rend)(size)(size_bytes)  //
                               (subspan));

#endif  // NTH_TEST_STD_TRACING_H
