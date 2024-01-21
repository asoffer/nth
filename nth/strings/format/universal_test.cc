#include "nth/strings/format/universal.h"

#include <string_view>

#include "nth/io/string_printer.h"
#include "nth/test/test.h"

namespace nth {
namespace {

void UniversalPrint(string_printer& p, auto const& value,
                    universal_formatter::options options = {
                        .depth    = 4,
                        .fallback = "...",
                    }) {
  universal_formatter f(options);
  f(p, value);
}

NTH_TEST("UniversalPrint/Builtin") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, "hello");
  UniversalPrint(p, '!');
  NTH_EXPECT(s == "hello!");
}

NTH_TEST("UniversalPrint/Ostream") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, 1234);
  NTH_EXPECT(s == "1234");
}

NTH_TEST("UniversalPrint/Bools") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, true);
  NTH_EXPECT(s == "true");
  s.clear();
  UniversalPrint(p, false);
  NTH_EXPECT(s == "false");
}

struct S {
  int32_t value;
};

NTH_TEST("UniversalPrint/Tuple") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, std::tuple<>{});
  NTH_EXPECT(s == "{}");
  s.clear();

  UniversalPrint(p, std::tuple<int>{});
  NTH_EXPECT(s == "{0}");
  s.clear();

  UniversalPrint(p, std::tuple<int, bool>{});
  NTH_EXPECT(s == "{0, false}");
  s.clear();
}

NTH_TEST("UniversalPrint/Optional") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, nullptr);
  NTH_EXPECT(s == "nullptr");
  s.clear();

  UniversalPrint(p, std::nullopt);
  NTH_EXPECT(s == "std::nullopt");
  s.clear();
  UniversalPrint(p, std::optional<int>());
  NTH_EXPECT(s == "std::nullopt");
  s.clear();
  UniversalPrint(p, std::optional<int>(3));
  NTH_EXPECT(s == "3");
}

struct Thing {
  friend void NthPrint(Printer auto& p, auto&, Thing) { p.write("thing"); }
};
NTH_TEST("UniversalPrint/NthPrint") {
  std::string s;
  string_printer p(s);
  Thing t;
  UniversalPrint(p, t);
  NTH_EXPECT(s == "thing");
}

NTH_TEST("UniversalPrint/Variant") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, std::variant<int, bool>(5));
  NTH_EXPECT(s == "5");
  s.clear();
  UniversalPrint(p, std::variant<int, bool>(true));
  NTH_EXPECT(s == "true");
}

NTH_TEST("UniversalPrint/Fallback") {
  std::string s;
  string_printer p(s);
  UniversalPrint(p, S{.value = 17});
  NTH_EXPECT(
      s ==
      "[Unprintable value of type nth::(anonymous namespace)::S: 11 00 00 00]");
}

NTH_TEST("UniversalPrint/ArrayLike") {
  int a[3] = {1, 2, 3};
  std::string s;
  string_printer p(s);
  UniversalPrint(p, a);
  NTH_EXPECT(s == "{1, 2, 3}");

  struct A {
    struct iter {
      int operator*() const { return value; }
      iter& operator++() {
        ++value;
        return *this;
      }
      bool operator==(iter const&) const = default;
      int value;
    };
    iter begin() const { return {3}; }
    iter end() const { return {6}; }
  };

  s.clear();
  UniversalPrint(p, A{});
  NTH_EXPECT(s == "{3, 4, 5}");
}

}  // namespace
}  // namespace nth
