#include "nth/debug/source_location.h"

#include <iostream>

namespace some_namespace {

static_assert([] {
#line 10
  return nth::source_location::current().line() == 10;
}());

static_assert([] {
#line 10 "some_file.h"
  return nth::source_location::current().file_name() == "some_file.h";
}());

constexpr std::string_view function_name() {
  return nth::source_location::current().function_name();
}
static_assert([] { return function_name() == "function_name"; }());

constexpr nth::source_location function_taking_default(
    nth::source_location loc = nth::source_location::current()) {
  return loc;
}
constexpr nth::source_location calls_default() {
#line 20 "some_other_file.h"
  return function_taking_default();
}

static_assert([] {
#line 30 "yet_another_file.h"
  auto loc = calls_default();
  return loc.file_name() == "some_other_file.h" and
         loc.function_name() == "calls_default" and loc.line() == 20;
}());

}  // namespace some_namespace

int main() { return 0; }
