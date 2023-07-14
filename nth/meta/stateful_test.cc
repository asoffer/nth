#include "nth/meta/stateful.h"

NTH_DEFINE_MUTABLE_COMPILE_TIME_SEQUENCE(s1);
NTH_DEFINE_MUTABLE_COMPILE_TIME_SEQUENCE(s2);

constexpr int evaluate() {
  s1.append<1>();
  s1.append<4>();
  static_assert(*s1 == nth::sequence<1, 4>);
  s1.append<9>();
  static_assert(*s1 == nth::sequence<1, 4, 9>);
  static_assert(*s2 == nth::sequence<>);
  s2.append<3>();
  static_assert(*s2 == nth::sequence<3>);
  static_assert(*s1 == nth::sequence<1, 4, 9>);
  return 0;
}

[[maybe_unused]] auto unused = evaluate();

int main() { return 0; }
