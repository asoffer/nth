#include "nth/debug/unreachable.h"

int main() {
  if (false) { NTH_UNREACHABLE(); }
  if (false) { NTH_UNREACHABLE(""); }
  if (false) { NTH_UNREACHABLE("{}") <<= {3}; }
  return 0;
}


