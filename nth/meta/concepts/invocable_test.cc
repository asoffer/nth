#include "nth/meta/concepts/invocable.h"

static_assert(nth::invocable<int()>);
static_assert(not nth::invocable<int(), int>);

static_assert(nth::invocable<void()>);
static_assert(nth::invocable<void(int), int>);
static_assert(not nth::invocable<void(int)>);

static_assert(nth::invocable<void(&)()>);
static_assert(nth::invocable<void(&)(int), int>);
static_assert(not nth::invocable<void(&)(int)>);

static_assert(nth::invocable<void(*)()>);
static_assert(nth::invocable<void(*)(int), int>);
static_assert(not nth::invocable<void(*)(int)>);

struct Uninvocable {};
static_assert(not nth::invocable<Uninvocable>);

struct InvocableWithInt {
  void operator()(int);
};
static_assert(not nth::invocable<InvocableWithInt>);
static_assert(nth::invocable<InvocableWithInt, int>);
static_assert(not nth::invocable<InvocableWithInt, int, int>);
static_assert(not nth::invocable<InvocableWithInt const>);
static_assert(not nth::invocable<InvocableWithInt const, int>);
static_assert(not nth::invocable<InvocableWithInt const, int, int>);

struct ConstInvocableWithInt {
  void operator()(int) const;
};

static_assert(not nth::invocable<ConstInvocableWithInt>);
static_assert(nth::invocable<ConstInvocableWithInt, int>);
static_assert(not nth::invocable<ConstInvocableWithInt, int, int>);
static_assert(not nth::invocable<ConstInvocableWithInt const>);
static_assert(nth::invocable<ConstInvocableWithInt const, int>);
static_assert(not nth::invocable<ConstInvocableWithInt const, int, int>);

struct InvocableWithAny {
  template <typename... Args>
  void operator()(Args&&...) {}
};
static_assert(nth::invocable<InvocableWithAny>);
static_assert(nth::invocable<InvocableWithAny, int>);
static_assert(nth::invocable<InvocableWithAny, int, bool, char[3]>);

int main() {}
