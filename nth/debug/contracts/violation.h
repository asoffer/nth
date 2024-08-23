#ifndef NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H
#define NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H

#include <string_view>

#include "nth/base/attributes.h"
#include "nth/debug/source_location.h"
#include "nth/format/format.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/writer/string.h"
#include "nth/io/writer/writer.h"
#include "nth/registration/registrar.h"

namespace nth {

struct contract {
  consteval contract(
      std::string_view category, std::string_view mode_path,
      std::string_view expr_str,
      struct source_location location = nth::source_location::current())
      : category_(category),
        mode_path_(mode_path),
        expr_str_(expr_str),
        source_location_(location) {}

  [[nodiscard]] constexpr std::string_view category() const {
    return category_;
  }

  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  [[nodiscard]] constexpr std::string_view mode_path() const {
    return mode_path_;
  }

  [[nodiscard]] constexpr std::string_view expression() const {
    return expr_str_;
  }

 private:
  std::string_view category_;
  std::string_view mode_path_;
  std::string_view expr_str_;
  struct source_location source_location_;
};

struct any_formattable_ref {
  struct vtable {
    std::string (*format)(void const*);
  };

  template <typename T>
  static constexpr vtable vtable_for{
      .format =
          [](void const* raw_self) {
            std::string s;
            nth::io::string_writer w(s);
            nth::format(w, {}, *reinterpret_cast<T const*>(raw_self));
            return s;
          },
  };

  template <typename T>
  constexpr any_formattable_ref(T const& t)
      : ptr_(nth::raw_address(t)), vtable_(&vtable_for<T>) {}

  friend format_spec<any_formattable_ref> NthFormatSpec(
      interpolation_string_view, type_tag<any_formattable_ref>) {
    return {};
  }

  friend void NthFormat(io::forward_writer auto& w,
                        format_spec<any_formattable_ref>,
                        any_formattable_ref ref) {
    w.write(nth::byte_range(ref.vtable_->format(ref.ptr_)));
  }

 private:
  void const* ptr_;
  vtable const* vtable_;
};

// `contract_violation`:
//
// Represents a failing result of a computation intended as verification during
// a test or debug-check.
struct contract_violation {
  contract_violation(contract const& c NTH_ATTRIBUTE(lifetimebound),
                     any_formattable_ref payload);

  [[nodiscard]] constexpr struct source_location source_location() const {
    return contract_.source_location();
  }

  // The category of the contract that failed (e.g., "NTH_ASSERT", or
  // "NTH_REQUIRE").
  constexpr std::string_view category() const { return contract_.category(); }

  // The stringified expression representing the violated contract.
  constexpr std::string_view expression() const {
    return contract_.expression();
  }
  constexpr any_formattable_ref payload() const { return payload_; }

 private:
  contract const& contract_;
  any_formattable_ref payload_;
};

// `register_contract_violation_handler`:
//
// Registers `handler` to be executed any time an expectation is evaluated (be
// that `NTH_REQUIRE`, `NTH_ENSURE`, `NTH_EXPECT`, or `NTH_ASSERT`). The
// execution order of handlers is unspecified and may be executed
// concurrently. Handlers cannot be un-registered.
void register_contract_violation_handler(
    void (*handler)(contract_violation const&));

// `registered_contract_violation_handlers`:
//
// Returns a view over the globally registered `contract_violation` handlers.
// Note that `ExpecattionResult` handlers may be registered at any time
// including during the execution of this function. The returned range will
// view all handlers registered at a particular instant. If another handler
// registration happens after the function returns, the returned
// `contract_violationHandlerRange` will not contain the newly registered
// handler.
registrar<void (*)(contract_violation const&)>::range_type
registered_contract_violation_handlers();

}  // namespace nth

#endif  // NTH_DEBUG_TRACE_CONTRACTS_VIOLATION_H
