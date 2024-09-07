#include "nth/io/writer/null.h"

#include "nth/io/writer/writer.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

NTH_TEST("null_writer/basic") {
  null_writer w;
  NTH_EXPECT(write_text(w, "Hello").written() == 5u);
  NTH_EXPECT(write_text(w, "").written() == 0u);
  NTH_EXPECT(write_text(w, "abcdefghijklmnopqrstuvwxyz").written() == 26u);
}

}  // namespace
}  // namespace nth::io
