# //nth/debug:unimplemented

This target defines the macro `NTH_UNIMPLEMENTED`. Invocations of this macro indicate that the
author intends this to be a valid code path that they have not yet implemented. Invocation of this
macro will always abort, after potentially logging. if `NTH_UNIMPLEMENTED` is invoked with no
arguments, a simple default message will be logged. Users may also provide an interpolation string
and inject extra information that will be appended to the log message, as if `NTH_UNREACHABLE` were
[`NTH_LOG`](/debug/log). That is, one could write

```
enum suit { clubs, diamonds, hearts, spades };

void handle_suit(suit s) {
  switch (s) {
      case clubs: return HandleClubs();
      case diamonds: return HandleDiamonds();
      case hearts: [[fallthrough]];
      case spades: NTH_UNIMPLEMENTED("Suit {} has not yet been implemented.") <<= {s};
  }
}
```
