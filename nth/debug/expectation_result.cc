#include "nth/debug/expectation_result.h"

#include "nth/base/indestructible.h"
#include "nth/registration/registrar.h"

namespace nth {
namespace {

using handler_type = void (*)(expectation_result const &);
indestructible<registrar<handler_type>> registrar_;

}  // namespace

void register_expectation_result_handler(handler_type handler) {
  registrar_->insert(handler);
}

expectation_result::expectation_result(nth::source_location location,
                                       bool success)
    : location_(location), success_(success) {}

expectation_result expectation_result::success(nth::source_location location) {
  return expectation_result(location, true);
}

expectation_result expectation_result::failure(nth::source_location location) {
  return expectation_result(location, false);
}

registrar<void (*)(expectation_result const &)>::range_type
registered_expectation_result_handlers() {
  return registrar_->registry();
}

}  // namespace nth
