#include "nth/types/reflect.h"

#include "nth/test/test.h"

namespace {

struct Empty {
  using nth_reflectable = void;
};

struct Base {};
struct EmptyWithBase : Base {
  using nth_reflectable = void;
};

struct EmptyWithMultipleBases : Base, Empty {
  using nth_reflectable = void;
};

struct One {
  using nth_reflectable = void;
  int n;
};

struct OneWithBase : Base {
  using nth_reflectable = void;
  int n;
};

struct Multiple {
  using nth_reflectable = void;
  int n;
  char c;
};

struct MultipleWithBase : Base {
  using nth_reflectable = void;
  int n;
  char c;
};

NTH_TEST("reflect/field_count") {
  NTH_EXPECT(nth::reflect::field_count<Empty, 0> == 0);
  NTH_EXPECT(nth::reflect::field_count<EmptyWithBase, 1> == 0);
  NTH_EXPECT(nth::reflect::field_count<EmptyWithMultipleBases, 2> == 0);
  NTH_EXPECT(nth::reflect::field_count<EmptyWithMultipleBases, 2> == 0);

  NTH_EXPECT(nth::reflect::field_count<One, 0> == 1);
  NTH_EXPECT(nth::reflect::field_count<OneWithBase, 1> == 1);

  NTH_EXPECT(nth::reflect::field_count<Multiple, 0> == 2);
  NTH_EXPECT(nth::reflect::field_count<MultipleWithBase, 1> == 2);
}

NTH_TEST("reflect/field_names") {
  Empty e;
  NTH_ASSERT(nth::reflect::field_names<0>(e) ==
             (std::array<std::string_view, 0>{}));
  EmptyWithBase eb;
  NTH_ASSERT(nth::reflect::field_names<1>(eb) ==
             (std::array<std::string_view, 0>{}));

  One o;
  NTH_ASSERT(nth::reflect::field_names<0>(o) ==
             (std::array<std::string_view, 1>{"n"}));
  OneWithBase ob;
  NTH_ASSERT(nth::reflect::field_names<1>(ob) ==
             (std::array<std::string_view, 1>{"n"}));

  Multiple m;
  NTH_ASSERT(nth::reflect::field_names<0>(m) ==
             (std::array<std::string_view, 2>{"n", "c"}));
  MultipleWithBase mb;
  NTH_ASSERT(nth::reflect::field_names<1>(mb) ==
             (std::array<std::string_view, 2>{"n", "c"}));
}

}  // namespace
