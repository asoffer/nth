#include "nth/strings/format/formatter.h"

#include "nth/io/printer.h"

struct NotAFormatter {};

struct AFormatter {
  void operator()(nth::Printer auto&, char) const;
};

static_assert(not nth::FormatterFor<NotAFormatter, bool>);
static_assert(not nth::FormatterFor<NotAFormatter, char>);
static_assert(not nth::FormatterFor<NotAFormatter, bool, char>);
static_assert(nth::FormatterFor<NotAFormatter>);

static_assert(nth::FormatterFor<AFormatter, bool>);
static_assert(nth::FormatterFor<AFormatter, char>);
static_assert(nth::FormatterFor<AFormatter, int>);
static_assert(nth::FormatterFor<AFormatter, int, char>);
static_assert(nth::FormatterFor<AFormatter>);

static_assert(nth::FormatterFor<nth::MinimalFormatterFor<bool>, bool>);
static_assert(nth::FormatterFor<nth::MinimalFormatterFor<char>, char>);
static_assert(nth::FormatterFor<nth::MinimalFormatterFor<int>, int>);
static_assert(not nth::FormatterFor<nth::MinimalFormatterFor<int>, int, char>);
static_assert(nth::FormatterFor<nth::MinimalFormatterFor<int, char>, int>);
static_assert(
    nth::FormatterFor<nth::MinimalFormatterFor<char, int>, int, char>);
static_assert(nth::FormatterFor<nth::MinimalFormatterFor<>>);

int main() { return 0; }
