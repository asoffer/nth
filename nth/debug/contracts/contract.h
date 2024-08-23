#ifndef NTH_DEBUG_CONTRACTS_CONTRACT_H
#define NTH_DEBUG_CONTRACTS_CONTRACT_H

#include <string_view>

#include "nth/debug/source_location.h"

namespace nth {

// `contract`:
// A contract represents a requirement in some portion of a program which must
// hold at the time it is evaluated for the program to be error-free.
struct contract {
  consteval contract(
      std::string_view category, std::string_view mode_path,
      std::string_view expr_str,
      struct source_location location = nth::source_location::current())
      : category_(category),
        mode_path_(mode_path),
        expr_str_(expr_str),
        source_location_(location) {}

  // A category associated with the contract. For contracts generated via macros
  // such as `NTH_REQUIRE`, this will be the stringification of the macro name
  // (e.g., "NTH_REQUIRE").
  [[nodiscard]] constexpr std::string_view category() const {
    return category_;
  }

  // The location at which the contract was constructed.
  [[nodiscard]] constexpr struct source_location source_location() const {
    return source_location_;
  }

  [[nodiscard]] constexpr std::string_view mode_path() const {
    return mode_path_;
  }

  // A string representation of the expression whose evaluation represents the
  // condition. Typically, for contracts generated via macros such as
  // `NTH_REQUIRE`, this is the stringification of the condition in the macro.
  [[nodiscard]] constexpr std::string_view expression() const {
    return expr_str_;
  }

 private:
  std::string_view category_;
  std::string_view mode_path_;
  std::string_view expr_str_;
  struct source_location source_location_;
};

}  // namespace nth

#endif  // NTH_DEBUG_CONTRACTS_CONTRACT_H
