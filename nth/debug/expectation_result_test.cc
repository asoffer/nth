#include "nth/debug/expectation_result.h"

#include "nth/debug/internal/raw_check.h"

size_t Count(nth::debug::ExpectationResultHandlerRange const &r) {
  size_t count = 0;
  for ([[maybe_unused]] auto unused : r) { ++count; }
  return count;
}

template <int>
void Handler(nth::debug::ExpectationResult const &) {}

int main() {
  auto handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 0);

  nth::debug::RegisterExpectationResultHandler(Handler<0>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 1);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  nth::debug::RegisterExpectationResultHandler(Handler<1>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 2);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<1>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  nth::debug::RegisterExpectationResultHandler(Handler<2>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 3);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<2>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<1>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  nth::debug::RegisterExpectationResultHandler(Handler<3>);
  nth::debug::RegisterExpectationResultHandler(Handler<4>);
  nth::debug::RegisterExpectationResultHandler(Handler<5>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 6);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<5>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<4>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<3>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<2>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<1>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  nth::debug::RegisterExpectationResultHandler(Handler<6>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 7);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<6>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<5>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<4>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<3>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<2>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<1>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  nth::debug::RegisterExpectationResultHandler(Handler<7>);
  nth::debug::RegisterExpectationResultHandler(Handler<8>);
  handlers = nth::debug::RegisteredExpectationResultHandlers();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(handlers) == 9);
  {
    auto iter = handlers.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<8>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<7>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<6>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<5>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<4>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<3>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<2>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<1>);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == Handler<0>);
  }

  return 0;
}
