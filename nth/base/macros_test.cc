#include "nth/base/macros.h"

#include <type_traits>

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

int main() { return 0; }
