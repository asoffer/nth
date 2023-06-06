#ifndef NTH_IO_ENVIRONMENT_VARIABLE_H
#define NTH_IO_ENVIRONMENT_VARIABLE_H

#include <stdlib.h>

#include <optional>
#include <string>

#include "nth/strings/null_termination.h"

namespace nth {
namespace internal_environment_variable {

std::optional<std::string> Get(char const *name);
void Unset(char const *name);
void Set(char const *name, char const *value);

}  // namespace internal_environment_variable

// These functions are used for interacting with environment variables. While
// the C++ standard provides `std::getenv`, it does not provide any mechanism
// for portably setting an environment variable, and for good reason: There is
// no thread-safe mechanism for modifying environment variables.
//
// These functions do provide thread-safe access to environment variables under
// the condition that environment variables are *only* accessed via these
// functions throughout the entire program. Any direct call to `std::getenv` or
// similar function interacting environment variables that is not synchronized
// with the functions defined below may race with these functions, meaning the
// behavior is undefined.
//
// In order to provide thread-safety, internally these functions synchronize,
// possibly taking a lock. For this reason it is usually more performant to only
// invoke `LoadEnvironmentVariable` when the variable's value may have changed
// in a way that should affect program behavior. If the value is not expected to
// change, or a change in the environment variable should not affect program
// behavior, it is usually more performant to store that string instead of
// reinvoking `LoadEnvironmentVariable`.

// Returns the string stored in the environment variable named `name` when
// invoked, if the environment variable is set. Otherwise, returns
// `std::nullopt`.
template <StringLike S>
std::optional<std::string> LoadEnvironmentVariable(S const &name) {
  return nth::InvokeOnNullTerminated(internal_environment_variable::Get, name);
}

// Sets the environment variable named `name` to the value `value`. If the
// overload accepting an optional is provided an unengaged optional, or for the
// overload taking `std::nullopt_t`, the environment variable is unset.
template <StringLike Name, StringLike Value>
void StoreEnvironmentVariable(Name const &name, Value const &value) {
  nth::InvokeOnNullTerminated(internal_environment_variable::Set, name, value);
}

template <StringLike Name>
void StoreEnvironmentVariable(Name const &name, std::nullopt_t) {
  nth::InvokeOnNullTerminated(internal_environment_variable::Unset, name);
}

template <StringLike Name, StringLike Value>
void StoreEnvironmentVariable(Name const &name,
                              std::optional<Value> const &value) {
  value ? nth::StoreEnvironmentVariable(name, *value)
        : nth::StoreEnvironmentVariable(name, std::nullopt);
}

}  // namespace nth

#endif  // NTH_IO_ENVIRONMENT_VARIABLE_H
