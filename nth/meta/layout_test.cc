#include "nth/meta/layout.h"

#include <cstdint>

constexpr size_t PtrBits = sizeof(void*) * CHAR_BIT;
template <size_t A, size_t B>
struct alignas(A) Type {
  char cs[B];
};

static_assert(nth::layout::of<int8_t> == nth::layout::of<uint8_t>);
static_assert(nth::layout::of<int64_t> == nth::layout::of<uint64_t>);
static_assert(nth::layout::of<int64_t> == nth::layout::of<int64_t const>);
static_assert(nth::layout::of<int64_t> == nth::layout::of<int64_t volatile>);
static_assert(nth::layout::of<int64_t> ==
              nth::layout::of<int64_t const volatile>);
static_assert(nth::layout::of<char>.bit_sequence() ==
              nth::layout::used<CHAR_BIT>);
static_assert(nth::layout::of<int>.bit_sequence() ==
              nth::layout::used<sizeof(int) * CHAR_BIT>);
static_assert(nth::layout::of<Type<4, 8>*>.bit_sequence() ==
              nth::layout::used<PtrBits - 2> + nth::layout::unset<2>);
static_assert(nth::layout::of<Type<32, 32>*>.alignment() == 32);
static_assert(nth::layout::of<Type<32, 32>*>.bit_sequence() ==
              nth::layout::used<PtrBits - 5> + nth::layout::unset<5>)
int main() { return 0; }
