#include "nth/format/format.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "nth/test/test.h"

namespace {

namespace some_ns {

struct S {
  friend void NthFormat(nth::io::writer auto& w, auto&, S const& s) {
    nth::io::write_text(w, "[[");
    nth::io::write_text(w, s.str);
    nth::io::write_text(w, "]]");
  }
  std::string str;
};

}  // namespace some_ns

NTH_TEST("format/absl_stringify") {
  NTH_EXPECT(absl::StrCat(some_ns::S{.str = "hello"}) == "[[hello]]");

  static_assert(absl::HasAbslStringify<some_ns::S>::value);

  // TODO: Understand why `absl::StrFormat` does not work.
  // NTH_EXPECT(absl::StrFormat("{%v}", some_ns::S{.str = "hello"}) ==
  //            "{[[hello]]}");
}

}  // namespace
