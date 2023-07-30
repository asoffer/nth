#ifndef NTH_PROCESS_SIGNAL_H
#define NTH_PROCESS_SIGNAL_H

#include <cstdint>

namespace nth {

// Represents a signal that can be sent to a process (such as `SIGABRT` or
// `SIGSEGV`).
struct signal {
  // Declarations of static constant members defining signals, such as
  // `signal::abort`, or `signal::segmentation_violation`. See the xmacro file
  // for a complete list of declared exit codes.
#define NTH_INTERNAL_XMACRO_SIGNAL(name, value) static signal const name;
#include "nth/process/signal.xmacro.h"
#undef NTH_INTERNAL_XMACRO_SIGNAL

  friend constexpr bool operator==(signal, signal) = default;
  friend constexpr bool operator!=(signal, signal) = default;

  // Returns the numeric value associated with the given signal.
  constexpr uint8_t code() const { return code_; }

 private:
  consteval signal(uint8_t n) : code_(n) {}

  uint8_t code_;
};

// Raises the signal represented by `s`.
int RaiseSignal(signal s);

}  // namespace nth

#endif  // NTH_PROCESS_SIGNAL_H
