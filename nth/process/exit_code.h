#ifndef NTH_PROCESS_EXIT_CODE_H
#define NTH_PROCESS_EXIT_CODE_H

#include <cstdint>

#include "nth/process/signal.h"
#include "nth/utility/early_exit.h"

namespace nth {

struct exit_code {
  // Declarations of static constant members defining exist codes, such as
  // `exit_code::success`, or `exit_code::generic_error`. See the xmacro file
  // for a complete list of declared exit codes.
#define NTH_INTERNAL_XMACRO_EXIT_CODE(name, value) static exit_code const name;
#include "nth/process/exit_code.xmacro.h"
#undef NTH_INTERNAL_XMACRO_EXIT_CODE

  // Constructs and `exit_code` from the given `signal`. Typically on posix
  // systems an error code whose value is greater than 128 is returned to
  // indicate the program exited with the signal whose value is 128 less than
  // the given exit code. That is, if a program exits due to `SIGABRT` (an abort
  // signal), the exit code will be 134 = 128 + 6, because `SIGABRT` has signal
  // value `6`.
  constexpr exit_code(signal s) : exit_code(128 + s.code()) {}

  // Returns the numeric value associated with the given exit code.
  constexpr uint8_t code() const { return code_; }

  friend bool operator==(exit_code, exit_code) = default;
  friend bool operator!=(exit_code, exit_code) = default;

  // Enable early-exit handling
  using promise_type = early_exit_promise_type<exit_code>;
  explicit exit_code(early_exit_constructor_t, exit_code*& v) { v = this; }
  explicit constexpr operator bool() const { return code_ == 0; }

 private:
  constexpr exit_code(uint8_t n) : code_(n) {}

  uint8_t code_;
};

}  // namespace nth

#endif  // NTH_PROCESS_EXIT_CODE_H
