#include "nth/utility/unconstructible.h"

struct S {
  static constexpr int function(nth::unconstructible_except_by<S> = {},
                                int x                             = 10) {
    return x;
  }

  static constexpr int calls_function(int x) { return function({}, x); }
};


static_assert(S::function() == 10);
static_assert(S::calls_function(5) == 5);

int main() { return 0; }
