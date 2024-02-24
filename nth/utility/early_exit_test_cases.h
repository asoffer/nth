#include "nth/test/test.h"
#include "nth/utility/early_exit.h"

NTH_TEST("nth/utility/early-exit", nth::Type auto t) {
  using type = nth::type_t<t>;
  static_assert(nth::supports_early_exit<type>);

  struct early_exit : nth::internal_early_exit::promise_return_type_base {
    nth::internal_early_exit::early_exit_constructor_t constructor = nth::
        internal_early_exit::promise_return_type_base::early_exit_constructor;
  };
  type *pointer;
  type object(early_exit{}.constructor, pointer);
  NTH_EXPECT(pointer == std::addressof(object));
}
