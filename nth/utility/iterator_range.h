#ifndef NTH_UTILITY_ITERATOR_RANGE_H
#define NTH_UTILITY_ITERATOR_RANGE_H

#include "nth/base/core.h"
#include "nth/debug/debug.h"
#include "nth/meta/concepts/core.h"

namespace nth {
namespace internal_iterator_range {

template <typename Iterator, int>
struct Base : private Iterator {
  explicit constexpr Base(Iterator&& iterator) : Iterator(NTH_MOVE(iterator)) {}
  explicit constexpr Base(Iterator const& iterator) : Iterator(iterator) {}

  constexpr Iterator const& iterator() const {
    return static_cast<Iterator const&>(*this);
  }
};

}  // namespace internal_iterator_range

template <typename B, typename E>
struct iterator_range : private internal_iterator_range::Base<B, 0>,
                        private internal_iterator_range::Base<E, 1> {
  using value_type     = nth::decayed<decltype(*nth::value<B>())>;
  using const_iterator = B;

  iterator_range() = default;

  iterator_range(B b, E e)
      : internal_iterator_range::Base<B, 0>(NTH_MOVE(b)),
        internal_iterator_range::Base<E, 1>(NTH_MOVE(e)) {}

  auto size() const requires requires(B b, E e) { e - b; }
  { return end() - begin(); }

  auto begin() const {
    return static_cast<internal_iterator_range::Base<B, 0> const&>(*this)
        .iterator();
  }
  auto end() const {
    return static_cast<internal_iterator_range::Base<E, 1> const&>(*this)
        .iterator();
  }

  bool empty() const { return begin() == end(); }
};

template <typename B, typename E>
iterator_range(B, E) -> iterator_range<B, E>;

}  // namespace nth

template <typename B, typename E>
NTH_TRACE_DECLARE_API_TEMPLATE((nth::iterator_range<B, E>),
                               (begin)(empty)(end));

#endif  // NTH_UTILITY_ITERATOR_RANGE_H
