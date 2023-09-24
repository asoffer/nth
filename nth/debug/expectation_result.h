#ifndef NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
#define NTH_DEBUG_TRACE_EXPECTATION_RESULT_H

#include "nth/debug/source_location.h"

namespace nth::debug {

// `ExpectationResult`:
//
// Represents the result of a computation intended as verification during a test
// or debug-check.
struct ExpectationResult {
  static ExpectationResult Success(nth::source_location location);
  static ExpectationResult Failure(nth::source_location location);

  bool success() const { return success_; }

  nth::source_location source_location() const { return location_; }

 private:
  ExpectationResult(nth::source_location location, bool success);

  nth::source_location location_;
  bool success_;
};

// `RegisterExpectationResultHandler`:
//
// Registers `handler` to be executed any time an expectation is evaluated (be
// that `NTH_REQUIRE`, `NTH_ENSURE`, `NTH_EXPECT`, or `NTH_ASSERT`). The
// execution order of handlers is unspecified and may be executed concurrently.
// Handlers cannot be un-registered.
void RegisterExpectationResultHandler(
    void (*handler)(ExpectationResult const&));

// `ExpectationResultHandlerRange`:
//
// Represents a collection of registered `ExpectationResult` handlers. which can
// be iterated over. Specifically, `ExpectationResultHandlerRange` adheres
struct ExpectationResultHandlerRange {
  struct const_iterator {
    using value_type = void (*)(ExpectationResult const&);

    const_iterator& operator++();
    const_iterator operator++(int);

    value_type operator*() const;
    value_type const* operator->() const;

    friend bool operator==(const_iterator, const_iterator) = default;
    friend bool operator!=(const_iterator, const_iterator) = default;

   private:
    friend ExpectationResultHandlerRange;
    friend ExpectationResultHandlerRange RegisteredExpectationResultHandlers();

    explicit const_iterator(uintptr_t const* ptr = nullptr) : ptr_(ptr) {}
    uintptr_t const* ptr_;
  };
  const_iterator begin() const { return begin_; }
  const_iterator end() const { return const_iterator(); }

 private:
  friend ExpectationResultHandlerRange RegisteredExpectationResultHandlers();
  ExpectationResultHandlerRange(const_iterator begin) : begin_(begin) {}
  const_iterator begin_;
};

// `RegisteredExpectationResultHandlers`:
//
// Returns a view over the globally registered `ExpectationResult` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will view
// all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `ExpectationResultHandlerRange` will not contain the newly registered
// handler.
ExpectationResultHandlerRange RegisteredExpectationResultHandlers();

}  // namespace nth::debug

#endif  // NTH_DEBUG_TRACE_EXPECTATION_RESULT_H
