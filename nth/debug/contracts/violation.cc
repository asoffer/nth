#include "nth/debug/contracts/violation.h"

#include "nth/base/indestructible.h"
#include "nth/registration/registrar.h"

namespace nth {
namespace {

using handler_type = void (*)(contract_violation const &);
indestructible<registrar<handler_type>> registrar_;

}  // namespace

void register_contract_violation_handler(handler_type handler) {
  registrar_->insert(handler);
}

contract_violation::contract_violation(contract const &c,
                                       any_formattable_ref payload)
    : contract_(c), payload_(payload) {}

registrar<void (*)(contract_violation const &)>::range_type
registered_contract_violation_handlers() {
  return registrar_->registry();
}

}  // namespace nth
