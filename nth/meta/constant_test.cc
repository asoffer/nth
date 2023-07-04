#include "nth/meta/constant.h"

#include "nth/meta/type.h"

namespace {

constexpr auto n = nth::constant<3>;
static_assert(n == 3);
static_assert(static_cast<int>(n) == 3);
static_assert(static_cast<bool>(n) == true);
static_assert(static_cast<double>(n) == 3.0);
static_assert(n.type() == nth::type<int>);

constexpr auto b = nth::constant<true>;
static_assert(b);
static_assert(static_cast<int>(b) == 1);
static_assert(b.type() == nth::type<bool>);

}  // namespace

int main() { return 0; }
