#include "nth/base/macros.h"

#include <string_view>
#include <type_traits>

#define NTH_INTERNAL_TEST_STRINGIFY(...)                                       \
  NTH_INTERNAL_TEST_STRINGIFY_IMPL(__VA_ARGS__)
#define NTH_INTERNAL_TEST_STRINGIFY_IMPL(...) #__VA_ARGS__

#define NTH_INTERNAL_TEST_EXPANSION_IS(expected, ...)                          \
  static_assert(                                                               \
      std::string_view(expected) == NTH_INTERNAL_TEST_STRINGIFY(__VA_ARGS__),  \
      "\n  Expected: " expected                                               \
      "\n  Actual:   " NTH_INTERNAL_TEST_STRINGIFY(__VA_ARGS__))

template <typename... Ts>
struct TypeList;

static_assert(std::is_same_v<NTH_TYPE(0, int), int>);
static_assert(std::is_same_v<NTH_TYPE(0, TypeList<int, bool, char>),
                             TypeList<int, bool, char>>);
static_assert(std::is_same_v<NTH_TYPE(1, TypeList<int, bool, char>, int), int>);

static_assert(NTH_TYPE_COUNT(int) == 1);
static_assert(NTH_TYPE_COUNT(TypeList<int, bool, char>) == 1);
static_assert(NTH_TYPE_COUNT(TypeList<int, bool, char>, int) == 2);

struct Incomplete;
struct PureVirtual {
  virtual void function() = 0;
};
static_assert(NTH_TYPE_COUNT(Incomplete, PureVirtual) == 2);
static_assert(std::is_same_v<NTH_TYPE(0, Incomplete&), Incomplete&>);
static_assert(std::is_same_v<NTH_TYPE(0, Incomplete&&), Incomplete&&>);

NTH_INTERNAL_TEST_EXPANSION_IS("a", NTH_IDENTITY(a));
NTH_INTERNAL_TEST_EXPANSION_IS("a, b, c", NTH_IDENTITY(a, b, c));

#define NTH_INTERNAL_TEST_ADD(x, y) x + y
static_assert(NTH_INVOKE(NTH_INTERNAL_TEST_ADD, 3, 4) ==
              NTH_INVOKE(NTH_INTERNAL_TEST_ADD, 5, 2));
#undef NTH_INTERNAL_TEST_ADD

NTH_INTERNAL_TEST_EXPANSION_IS("true", NTH_IS_EMPTY());
NTH_INTERNAL_TEST_EXPANSION_IS("false", NTH_IS_EMPTY(x));
NTH_INTERNAL_TEST_EXPANSION_IS("false", NTH_IS_EMPTY(x, y, z));

NTH_INTERNAL_TEST_EXPANSION_IS("false", NTH_NOT(NTH_IS_EMPTY()));
NTH_INTERNAL_TEST_EXPANSION_IS("true", NTH_NOT(NTH_IS_EMPTY(x)));

NTH_INTERNAL_TEST_EXPANSION_IS("x()", NTH_IF(true, x, y));
NTH_INTERNAL_TEST_EXPANSION_IS("y()", NTH_IF(false, x, y));
NTH_INTERNAL_TEST_EXPANSION_IS("x(1)", NTH_IF(true, x, y, 1));
NTH_INTERNAL_TEST_EXPANSION_IS("y(1)", NTH_IF(false, x, y, 1));
NTH_INTERNAL_TEST_EXPANSION_IS("x(1, 2, 3)", NTH_IF(true, x, y, 1, 2, 3));
NTH_INTERNAL_TEST_EXPANSION_IS("y(1, 2, 3)", NTH_IF(false, x, y, 1, 2, 3));

NTH_INTERNAL_TEST_EXPANSION_IS("a", NTH_IGNORE_PARENTHESES(a));
NTH_INTERNAL_TEST_EXPANSION_IS("b, c", NTH_IGNORE_PARENTHESES((b, c)));
NTH_INTERNAL_TEST_EXPANSION_IS("(d, e, f)",
                               NTH_IGNORE_PARENTHESES(((d, e, f))));
NTH_INTERNAL_TEST_EXPANSION_IS("(a + b) * c",
                               NTH_IGNORE_PARENTHESES((a + b) * c));

NTH_INTERNAL_TEST_EXPANSION_IS("a", NTH_FIRST_ARGUMENT(a));
NTH_INTERNAL_TEST_EXPANSION_IS("b", NTH_FIRST_ARGUMENT(b, c));
NTH_INTERNAL_TEST_EXPANSION_IS("d", NTH_FIRST_ARGUMENT(d, e, f));

NTH_INTERNAL_TEST_EXPANSION_IS("c", NTH_SECOND_ARGUMENT(b, c));
NTH_INTERNAL_TEST_EXPANSION_IS("e", NTH_SECOND_ARGUMENT(d, e, f));

NTH_INTERNAL_TEST_EXPANSION_IS("false", NTH_IS_PARENTHESIZED(a));
NTH_INTERNAL_TEST_EXPANSION_IS("true", NTH_IS_PARENTHESIZED((b)));
NTH_INTERNAL_TEST_EXPANSION_IS("true", NTH_IS_PARENTHESIZED((c, d)));
NTH_INTERNAL_TEST_EXPANSION_IS("true", NTH_IS_PARENTHESIZED(((e))));
NTH_INTERNAL_TEST_EXPANSION_IS("false", NTH_IS_PARENTHESIZED((f + g) * h));

NTH_INTERNAL_TEST_EXPANSION_IS(R"("")", NTH_STRINGIFY());
NTH_INTERNAL_TEST_EXPANSION_IS(R"("x")", NTH_STRINGIFY(x));
NTH_INTERNAL_TEST_EXPANSION_IS(R"("\"\"")", NTH_STRINGIFY(""));

#undef NTH_INTERNAL_TEST_STRINGIFY_IMPL
#undef NTH_INTERNAL_TEST_STRINGIFY

int main() { return 0; }
