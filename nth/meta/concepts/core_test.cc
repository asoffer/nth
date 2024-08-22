#include "nth/meta/concepts/core.h"

struct A {};
struct B {};
struct C : B {};

static_assert(nth::precisely<A, A>);
static_assert(not nth::precisely<A, B>);
static_assert(not nth::precisely<A, C>);
static_assert(not nth::precisely<B, C>);
static_assert(nth::precisely<A*, A*>);
static_assert(not nth::precisely<A*, B*>);
static_assert(not nth::precisely<A*, C*>);
static_assert(not nth::precisely<B*, C*>);

static_assert(nth::any_of<int, int>);
static_assert(not nth::any_of<int, bool>);
static_assert(not nth::any_of<int>);
static_assert(nth::any_of<int, bool, int>);
static_assert(nth::any_of<int, int, int>);
static_assert(not nth::any_of<int, bool, char>);

enum E {};
enum class EC {};

static_assert(nth::enumeration<E>);
static_assert(not nth::enumeration<A>);
static_assert(nth::enumeration<EC>);

static_assert(nth::decays_to<int&, int>);
static_assert(nth::decays_to<int const volatile&, int>);
static_assert(nth::decays_to<void(), void (*)()>);
static_assert(nth::decays_to<int[3], int*>);
static_assert(nth::decays_to<int const (&)[3], int const *>);
static_assert(nth::decays_to<int volatile (&)[3], int volatile *>);

static_assert(nth::lvalue_reference<int &>);
static_assert(not nth::rvalue_reference<int &>);
static_assert(nth::reference<int &>);
static_assert(not nth::by_value<int &>);

static_assert(nth::lvalue_reference<int const &>);
static_assert(not nth::rvalue_reference<int const &>);
static_assert(nth::reference<int const &>);
static_assert(not nth::by_value<int const &>);

static_assert(not nth::lvalue_reference<int &&>);
static_assert(nth::rvalue_reference<int &&>);
static_assert(nth::reference<int &&>);
static_assert(not nth::by_value<int &&>);

static_assert(not nth::lvalue_reference<int const &&>);
static_assert(nth::rvalue_reference<int const &&>);
static_assert(nth::reference<int const &&>);
static_assert(not nth::by_value<int const &&>);

static_assert(not nth::lvalue_reference<int>);
static_assert(not nth::rvalue_reference<int>);
static_assert(not nth::reference<int>);
static_assert(nth::by_value<int>);

static_assert(not nth::lvalue_reference<int const>);
static_assert(not nth::rvalue_reference<int const>);
static_assert(not nth::reference<int const>);
static_assert(nth::by_value<int const>);

int main() {}
