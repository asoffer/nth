#include "nth/test/arguments.h"

namespace nth {

static_assert(std::is_same_v<decltype(TestArguments(std::declval<int>())),
                             TestArguments<int>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &>())),
                             TestArguments<int &>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &&>())),
                             TestArguments<int>>);

static_assert(std::is_same_v<decltype(TestArguments(std::declval<int>(),
                                                    std::declval<bool>())),
                             TestArguments<int, bool>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &>(),
                                                    std::declval<bool>())),
                             TestArguments<int &, bool>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &&>(),
                                                    std::declval<bool>())),
                             TestArguments<int, bool>>);

static_assert(std::is_same_v<decltype(TestArguments(std::declval<int>(),
                                                    std::declval<bool &>())),
                             TestArguments<int, bool &>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &>(),
                                                    std::declval<bool &>())),
                             TestArguments<int &, bool &>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &&>(),
                                                    std::declval<bool &>())),
                             TestArguments<int, bool &>>);

static_assert(std::is_same_v<decltype(TestArguments(std::declval<int>(),
                                                    std::declval<bool &&>())),
                             TestArguments<int, bool>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &>(),
                                                    std::declval<bool &&>())),
                             TestArguments<int &, bool>>);
static_assert(std::is_same_v<decltype(TestArguments(std::declval<int &&>(),
                                                    std::declval<bool &&>())),
                             TestArguments<int, bool>>);
}  // namespace nth

int main() { return 0; }
