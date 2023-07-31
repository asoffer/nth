#include "nth/debug/unimplemented.h"

int main() {
  if (false) { NTH_UNIMPLEMENTED(); }
  if (false) { NTH_UNIMPLEMENTED(""); }
  if (false) { NTH_UNIMPLEMENTED("{}") <<= {3}; }
  return 0;
}
