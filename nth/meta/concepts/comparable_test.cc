#include "nth/meta/concepts/comparable.h"

struct Nothing {};

struct FriendEq {
  friend bool operator==(FriendEq, FriendEq);
};

struct FriendEqBadReturn {
  friend int operator==(FriendEqBadReturn, FriendEqBadReturn);
};

struct MemberEq {
  bool operator==(MemberEq) const;
};

struct FriendCompare {
  friend bool operator<=(FriendCompare, FriendCompare);
  friend bool operator<(FriendCompare, FriendCompare);
  friend bool operator>(FriendCompare, FriendCompare);
  friend bool operator>=(FriendCompare, FriendCompare);
  friend bool operator==(FriendCompare, FriendCompare);
};

static_assert(not nth::comparable<Nothing>);
static_assert(not nth::equality_comparable<Nothing>);
static_assert(not nth::comparable<FriendEq>);
static_assert(nth::equality_comparable<FriendEq>);
static_assert(not nth::comparable<FriendEqBadReturn>);
static_assert(not nth::equality_comparable<FriendEqBadReturn>);
static_assert(not nth::comparable<MemberEq>);
static_assert(nth::equality_comparable<MemberEq>);
static_assert(nth::comparable<FriendCompare>);
static_assert(nth::equality_comparable<FriendCompare>);

int main() {}
