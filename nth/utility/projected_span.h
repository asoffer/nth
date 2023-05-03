#ifndef NTH_UTILITY_PROJECTED_SPAN_H
#define NTH_UTILITY_PROJECTED_SPAN_H

#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>

#include "nth/meta/type.h"

namespace nth {

// Represents a view over a contiguous range of objects that, upon
// viewing (via `operator*` on an iterator) invokes the call operator on a
// projection.
template <auto Projection>
struct ProjectedSpan {
  private:
   static constexpr auto projection_type =
       nth::type<std::remove_reference_t<decltype(*+Projection)>>;

  public:
   using viewed_type =
       nth::type_t<projection_type.parameters().template get<0>().decayed()>;
   using value_type = nth::type_t<projection_type.return_type()>;

   // Constructs an empty span whose data pointer is null.
   explicit ProjectedSpan() : ptr_(nullptr), size_(0) {}

   // Constructs a span consisting of all elements in the half-open range
   // starting at `b` (inclusive) and ending at `e` (exclusive).
   template <std::contiguous_iterator Iter>
   explicit ProjectedSpan(Iter b, Iter e)
       : ptr_(std::addressof(*b)),
         size_(std::addressof(*e) - std::addressof(*b)) {}

   // Constructs a span consisting of all elements in the range from `
   // starting at `b` (inclusive) and ending at `e` (exclusive).
   ProjectedSpan(auto &&c)
       : ProjectedSpan(
             [&]() {
               using std::begin;
               return begin(c);
             }(),
             [&]() {
               using std::end;
               return end(c);
             }()) {}

   // Iterator over the spanned elements. Invoking `operator*` on this iterator
   // invokes `Projection` on a reference to the underlying value referred to by
   // the iterator.
   struct const_iterator {
     constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
     constexpr const_iterator operator++() { return const_iterator{++ptr_}; }
     constexpr const_iterator operator++(int) { return const_iterator{ptr_++}; }
     constexpr const_iterator operator--() { return const_iterator{--ptr_}; }
     constexpr const_iterator operator--(int) { return const_iterator{ptr_--}; }
     friend constexpr const_iterator operator+(const_iterator lhs,
                                               ptrdiff_t rhs) {
       return const_iterator{lhs.ptr_ + rhs};
     }
     friend constexpr const_iterator operator+(ptrdiff_t lhs,
                                               const_iterator rhs) {
       return rhs + lhs;
     }
     friend constexpr const_iterator operator-(const_iterator lhs,
                                               ptrdiff_t rhs) {
       return const_iterator{lhs.ptr_ - rhs};
     }
     friend constexpr const_iterator operator-(ptrdiff_t lhs,
                                               const_iterator rhs) {
       return rhs + lhs;
     }

     friend constexpr bool operator==(const_iterator, const_iterator) = default;
     friend constexpr bool operator!=(const_iterator, const_iterator) = default;

    private:
     friend struct ProjectedSpan<Projection>;
     constexpr explicit const_iterator(viewed_type const *ptr) : ptr_(ptr) {}
     viewed_type const *ptr_ = nullptr;
  };

  // Reverse iterator over the spanned elements. Invoking `operator*` on this
  // iterator invokes `Projection` on a reference to the underlying value
  // referred to by the iterator.
  struct const_reverse_iterator {
    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr const_reverse_iterator operator++() {
      return const_reverse_iterator{--ptr_};
    }
    constexpr const_reverse_iterator operator++(int) {
      return const_reverse_iterator{ptr_--};
    }
    constexpr const_reverse_iterator operator--() {
      return const_reverse_iterator{++ptr_};
    }
    constexpr const_reverse_iterator operator--(int) {
      return const_reverse_iterator{ptr_++};
    }
    friend constexpr const_reverse_iterator operator+(
        const_reverse_iterator lhs, ptrdiff_t rhs) {
      return const_reverse_iterator{lhs.ptr_ - rhs};
    }
    friend constexpr const_reverse_iterator operator+(
        ptrdiff_t lhs, const_reverse_iterator rhs) {
      return rhs + lhs;
    }
    friend constexpr const_reverse_iterator operator-(
        const_reverse_iterator lhs, ptrdiff_t rhs) {
      return const_reverse_iterator{lhs.ptr_ + rhs};
    }
    friend const_reverse_iterator operator-(ptrdiff_t lhs,
                                            const_reverse_iterator rhs) {
      return rhs - lhs;
    }

    friend constexpr bool operator==(const_reverse_iterator,
                                     const_reverse_iterator) = default;
    friend constexpr bool operator!=(const_reverse_iterator,
                                     const_reverse_iterator) = default;

   private:
    friend struct ProjectedSpan<Projection>;
    constexpr explicit const_reverse_iterator(viewed_type const *ptr)
        : ptr_(ptr) {}
    viewed_type const *ptr_ = nullptr;
  };

  constexpr auto cbegin() const { return const_iterator{ptr_}; }
  constexpr auto cend() const { return const_iterator{ptr_ + size_}; }
  constexpr auto begin() const { return cbegin(); }
  constexpr auto end() const { return cend(); }
  constexpr auto rbegin() const { return crbegin(); }
  constexpr auto rend() const { return crend(); }
  constexpr auto crbegin() const {
    return const_reverse_iterator{ptr_ + (size_ - 1)};
  }
  constexpr auto crend() const { return const_reverse_iterator{ptr_ - 1}; }

  // Returns the number of elements viewed by the span.
  constexpr size_t size() const { return size_; }

  // Returns true if and only if the span contains no elements.
  constexpr bool empty() const { return size_ == 0; }

  // Returns a pointer to the data referenced by the span. Note that if the span
  // is empty this may still be non-null.
  constexpr viewed_type const *data() const { return ptr_; }

  // Returns the projection of the first element. Behavior is undefined if the
  // span is empty.
  constexpr decltype(auto) front() const {
    assert(not empty());
    return *begin();
  }

  // Returns the projection of the last element. Behavior is undefined if the
  // span is empty.
  constexpr decltype(auto) back() const {
    assert(not empty());
    return *rbegin();
  }

  constexpr void remove_prefix(size_t num) {
    assert(num <= size());
    ptr_ += num;
    size_ -= num;
  }

  constexpr void remove_suffix(size_t num) {
    assert(num <= size());
    size_ -= num;
  }

  constexpr decltype(auto) operator[](size_t n) const {
    assert(n < size());
    return *(begin() + n);
  }

 private:
  viewed_type *ptr_ = nullptr;
  size_t size_      = 0;
};

}  // namespace nth

#endif  // NTH_UTILITY_PROJECTED_SPAN_H
