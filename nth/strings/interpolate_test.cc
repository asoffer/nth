#include "nth/strings/interpolate.h"

#include "nth/io/string_printer.h"
#include "nth/strings/format/formatter.h"
#include "nth/strings/format/universal.h"
#include "nth/test/test.h"

namespace nth {
namespace {

#if 0
TODO: Death tests are not yet supported.
NTH_TEST("interpolation_string/construction") {
  NTH_EXPECT([] { interpolation_string("{"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{x}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{{}"); } >>= nth::AbortsExecution());
  NTH_EXPECT([] { interpolation_string("{}{x"); } >>= nth::AbortsExecution());
  [[maybe_unused]] constexpr interpolation_string f1("{}");
  [[maybe_unused]] constexpr interpolation_string f2("{}{}");
}

NTH_TEST("interpolation_string/utf8-construction") {
  [[maybe_unused]] constexpr interpolation_string f1("\xc0\x80");
  [[maybe_unused]] constexpr interpolation_string f2("\xc0\x80{}");
  NTH_EXPECT([] { interpolation_string("\xc0\x80}"); } >>=
             nth::AbortsExecution());
}
#endif

struct TrivialFormatter {
  void operator()(nth::Printer auto& p, std::string_view s) const {
    p.write(s);
  }
};

NTH_TEST("interpolation_string/printer") {
  std::string s;
  string_printer p(s);
  TrivialFormatter t;
  Interpolate<"abc{}def">(p, t, "xyz");
  NTH_EXPECT(s == "abcxyzdef");
  s.clear();

  Interpolate<"abc{}def">(p, t, "12345");
  NTH_EXPECT(s == "abc12345def");
  s.clear();

  Interpolate<"abc">(p, t);
  NTH_EXPECT(s == "abc");
  s.clear();

  Interpolate<"{}abc{}{}">(p, t, "1", "2", "3");
  NTH_EXPECT(s == "1abc23");
  s.clear();

  Interpolate<"{}años">(p, t, "cumple");
  NTH_EXPECT(s == "cumpleaños");
}

NTH_TEST("interpolation_string/universal-formatter") {
  std::string s;
  string_printer p(s);
  universal_formatter f({
      .depth    = 3,
      .fallback = "...",
  });
  Interpolate<"abc{}def">(p, f, false);
  NTH_EXPECT(s == "abcfalsedef");
  s.clear();

  Interpolate<"abc{}def">(p, f, true);
  NTH_EXPECT(s == "abctruedef");
  s.clear();

  Interpolate<"abc">(p, f);
  NTH_EXPECT(s == "abc");
  s.clear();

  Interpolate<"{}abc{}{}">(p, f, 1, 2, 3);
  NTH_EXPECT(s == "1abc23");
  s.clear();

  Interpolate<"{}años">(p, f, "cumple");
  NTH_EXPECT(s == "cumpleaños");
}

}  // namespace
}  // namespace nth
