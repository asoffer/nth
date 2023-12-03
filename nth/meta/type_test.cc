#include "nth/meta/type.h"

#include <optional>
#include <sstream>
#include <string>
#include <variant>

struct SomeStruct {};

namespace {

static_assert(nth::type<int> == nth::type<int>);
static_assert(not(nth::type<int> == nth::type<bool>));
static_assert(nth::type<bool> == nth::type<bool>);
static_assert(not(nth::type<int> != nth::type<int>));
static_assert(nth::type<int> != nth::type<bool>);
static_assert(not(nth::type<bool> != nth::type<bool>));

// Decay
static_assert(nth::type<int>.decayed() == nth::type<int>);
static_assert(nth::type<int &>.decayed() == nth::type<int>);
static_assert(nth::type<int &&>.decayed() == nth::type<int>);
static_assert(nth::type<int const>.decayed() == nth::type<int>);
static_assert(nth::type<int const &>.decayed() == nth::type<int>);
static_assert(nth::type<int const &&>.decayed() == nth::type<int>);
static_assert(nth::type<int volatile &>.decayed() == nth::type<int>);
static_assert(nth::type<int[3]>.decayed() == nth::type<int *>);
static_assert(nth::type<int (&)[3]>.decayed() == nth::type<int *>);
static_assert(nth::type<int(bool)>.decayed() == nth::type<int (*)(bool)>);
static_assert(nth::type<int(bool (&)[3])>.decayed() ==
              nth::type<int (*)(bool (&)[3])>);

// cast
static_assert(nth::type<int>.cast(true) == 1);
struct A {};
struct B {
  constexpr B(A) : value(17) {}
  int value;
};
static_assert(nth::type<B>.cast(A{}).value == 17);

// type_t
static_assert(nth::type<nth::type_t<nth::type<int>>> == nth::type<int>);
static_assert(nth::type<nth::type_t<nth::type<bool>>> == nth::type<bool>);

// is_a
static_assert(not nth::type<int>.is_a<std::optional>());
static_assert(nth::type<std::optional<int>>.is_a<std::optional>());
static_assert(not nth::type<std::variant<int>>.is_a<std::optional>());
static_assert(nth::type<std::variant<int, bool>>.is_a<std::variant>());
static_assert(not nth::type<std::optional<int>>.is_a<std::variant>());
static_assert(nth::type<std::string>.is_a<std::basic_string>());

// Dependent
static_assert(nth::type<int>.dependent(true));
static_assert(not nth::type<int>.dependent(false));

// Construct but never invoke an expression that would static_assert false to
// prove that the dependent value is indeed dependent.
template <typename T>
void Dependent() {
  static_assert(nth::type<T>.dependent(false));
}

// Function types
static_assert(nth::type<int()>.return_type() == nth::type<int>);
static_assert(nth::type<void()>.return_type() == nth::type<void>);
static_assert((nth::type<void(int, bool)>.return_type()) == nth::type<void>);

static_assert(nth::type<int()>.parameters() == nth::type_sequence<>);
static_assert(nth::type<void()>.parameters() == nth::type_sequence<>);
static_assert(nth::type<void(void)>.parameters() == nth::type_sequence<>);
static_assert(nth::type<void(int)>.parameters() == nth::type_sequence<int>);
static_assert(nth::type<void(int, bool)>.parameters() ==
              nth::type_sequence<int, bool>);

// Without qualifiers
static_assert(nth::type<int>.without_const() == nth::type<int>);
static_assert(nth::type<int const>.without_const() == nth::type<int>);
static_assert(nth::type<int const volatile>.without_const() ==
              nth::type<int volatile>);
static_assert(nth::type<int const &>.without_const() == nth::type<int const &>);

static_assert(nth::type<int>.without_volatile() == nth::type<int>);
static_assert(nth::type<int volatile>.without_volatile() == nth::type<int>);
static_assert(nth::type<int const volatile>.without_volatile() ==
              nth::type<int const>);
static_assert(nth::type<int volatile &>.without_volatile() ==
              nth::type<int volatile &>);

static_assert(nth::type<int>.without_reference() == nth::type<int>);
static_assert(nth::type<int &>.without_reference() == nth::type<int>);
static_assert(nth::type<int &&>.without_reference() == nth::type<int>);
static_assert(nth::type<int const &>.without_reference() ==
              nth::type<int const>);
static_assert(nth::type<int const &&>.without_reference() ==
              nth::type<int const>);

}  // namespace

int main() {
  auto to_string = [](nth::Type auto t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
  };

  if (to_string(nth::type<int>) != "int") { return 1; }
  if (to_string(nth::type<int **>) != "int **") { return 1; }
  if (to_string(nth::type<const int *const *>) != "const int *const *") {
    return 1;
  }
  if (to_string(nth::type<SomeStruct>) != "SomeStruct") { return 1; }

  auto t1         = nth::type<int>;
  auto t2         = nth::type<int>;
  auto t3         = t1;
  nth::TypeId id1 = t1;
  nth::TypeId id2 = t2;
  nth::TypeId id3 = t3;
  if (id1 != id2) { return 1; }
  if (id2 != id3) { return 1; }
  if (id1 != id3) { return 1; }

  std::stringstream ss;
  ss << t1;
  if (ss.str() != "int") { return 1; }
  return 0;
}
