#ifndef NTH_UTILITY_PROJECTED_SPAN_H
#define NTH_UTILITY_PROJECTED_SPAN_H

#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>

#include "nth/meta/type.h"

namespace nth {
template <typename, auto>
struct ProjectedSpan;

namespace internal_projected_span {

template <typename T, auto Projection>
struct ConditionallyExposeIterator {};

template <typename T, auto Projection>
requires(not std::is_const_v<T> and not requires {
  Projection(std::declval<std::add_const_t<T> &>());
}) struct ConditionallyExposeIterator<T, Projection> {
 private:
  static constexpr auto projection_type =
      nth::type<std::remove_reference_t<decltype(*+Projection)>>;

  using viewed_type =
      nth::type_t<projection_type.parameters().template get<0>().decayed()>;

 public:
  // Iterator over the spanned elements. Invoking `operator*` on this iterator
  // invokes `Projection` on a reference to the underlying value referred to by
  // the iterator.
  struct iterator {
    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr iterator operator++() { return iterator{++ptr_}; }
    constexpr iterator operator++(int) { return iterator{ptr_++}; }
    constexpr iterator operator--() { return iterator{--ptr_}; }
    constexpr iterator operator--(int) { return iterator{ptr_--}; }
    friend constexpr iterator operator+(iterator lhs, ptrdiff_t rhs) {
      return iterator{lhs.ptr_ + rhs};
    }
    friend constexpr iterator operator+(ptrdiff_t lhs, iterator rhs) {
      return rhs + lhs;
    }
    friend constexpr iterator operator-(iterator lhs, ptrdiff_t rhs) {
      return iterator{lhs.ptr_ - rhs};
    }
    friend constexpr iterator operator-(ptrdiff_t lhs, iterator rhs) {
      return rhs + lhs;
    }

    friend constexpr bool operator==(iterator, iterator) = default;
    friend constexpr bool operator!=(iterator, iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit iterator(viewed_type *ptr) : ptr_(ptr) {}
    viewed_type *ptr_ = nullptr;
  };

  struct reverse_iterator {
    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr reverse_iterator operator++() { return reverse_iterator{--ptr_}; }
    constexpr reverse_iterator operator++(int) {
      return reverse_iterator{ptr_--};
    }
    constexpr reverse_iterator operator--() { return reverse_iterator{++ptr_}; }
    constexpr reverse_iterator operator--(int) {
      return reverse_iterator{ptr_++};
    }
    friend constexpr reverse_iterator operator+(reverse_iterator lhs,
                                                ptrdiff_t rhs) {
      return reverse_iterator{lhs.ptr_ - rhs};
    }
    friend constexpr reverse_iterator operator+(ptrdiff_t lhs,
                                                reverse_iterator rhs) {
      return rhs + lhs;
    }
    friend constexpr reverse_iterator operator-(reverse_iterator lhs,
                                                ptrdiff_t rhs) {
      return reverse_iterator{lhs.ptr_ + rhs};
    }
    friend reverse_iterator operator-(ptrdiff_t lhs, reverse_iterator rhs) {
      return rhs - lhs;
    }

    friend constexpr bool operator==(reverse_iterator,
                                     reverse_iterator) = default;
    friend constexpr bool operator!=(reverse_iterator,
                                     reverse_iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit reverse_iterator(viewed_type *ptr) : ptr_(ptr) {}
    viewed_type *ptr_ = nullptr;
  };
};

template <typename, auto>
struct ConditionallyExposeConstIterator {};

template <typename T, auto Projection>
requires(requires { Projection(std::declval<std::add_const_t<T> &>()); })  //
    struct ConditionallyExposeConstIterator<T, Projection> {
 private:
  static constexpr auto projection_type =
      nth::type<std::remove_reference_t<decltype(*+Projection)>>;

  using viewed_type =
      nth::type_t<projection_type.parameters().template get<0>().decayed()>;

 public:
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
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit const_iterator(viewed_type *ptr) : ptr_(ptr) {}
    viewed_type *ptr_ = nullptr;
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
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit const_reverse_iterator(viewed_type *ptr) : ptr_(ptr) {}
    viewed_type *ptr_ = nullptr;
  };
};

}  // namespace internal_projected_span

// Represents a view over a contiguous range of objects that, upon
// viewing (via `operator*` on an iterator) invokes the call operator on a
// projection.
template <typename T, auto Projection>
struct ProjectedSpan
    : internal_projected_span::ConditionallyExposeIterator<T, Projection>,
      internal_projected_span::ConditionallyExposeConstIterator<T, Projection> {
 private:
  static constexpr auto projection_type =
      nth::type<std::remove_reference_t<decltype(*+Projection)>>;

  using self_type = ProjectedSpan<T, Projection>;

  static constexpr bool exposes_iterator =
      not std::is_const_v<T> and not requires {
    Projection(std::declval<std::add_const_t<T> &>());
  };
  static constexpr bool exposes_const_iterator = requires {
    Projection(std::declval<std::add_const_t<T> &>());
  };
  static_assert(exposes_iterator or exposes_const_iterator);

 public:
  using viewed_type = T;
  using value_type  = nth::type_t<projection_type.return_type()>;

  // Constructs an empty span whose data pointer is null.
  explicit ProjectedSpan() = default;

  // Constructs a span consisting of all elements in the half-open range
  // starting at `b` (inclusive) and ending at `e` (exclusive).
  template <std::contiguous_iterator Iter>
  explicit ProjectedSpan(Iter b, Iter e)
      : ptr_(std::addressof(*b)),
        size_(std::addressof(*e) - std::addressof(*b)) {}

  // Constructs a span consisting of all elements in the range starting at `b`
  // (inclusive) and ending at `e` (exclusive).
  ProjectedSpan(auto &c)
      : ProjectedSpan(
            [&]() {
              using std::begin;
              return begin(c);
            }(),
            [&]() {
              using std::end;
              return end(c);
            }()) {}

  constexpr auto cbegin() const requires(exposes_const_iterator) {
    return typename self_type::const_iterator{ptr_};
  }
  constexpr auto cend() const requires(exposes_const_iterator) {
    return typename self_type::const_iterator{ptr_ + size_};
  }
  constexpr auto begin() const requires(exposes_const_iterator) {
    return typename self_type::const_iterator{ptr_};
  }
  constexpr auto end() const requires(exposes_const_iterator) {
    return typename self_type::const_iterator{ptr_ + size_};
  }
  constexpr auto begin() requires(exposes_iterator) {
    return typename self_type::iterator{ptr_};
  }
  constexpr auto end() requires(exposes_iterator) {
    return typename self_type::iterator{ptr_ + size_};
  }
  constexpr auto crbegin() const requires(exposes_const_iterator) {
    return typename self_type::const_reverse_iterator{ptr_ + (size_ - 1)};
  }
  constexpr auto crend() const requires(exposes_const_iterator) {
    return typename self_type::const_reverse_iterator{ptr_ - 1};
  }
  constexpr auto rbegin() const requires(exposes_const_iterator) {
    return typename self_type::reverse_iterator{ptr_ + (size_ - 1)};
  }
  constexpr auto rend() const requires(exposes_const_iterator) {
    return typename self_type::reverse_iterator{ptr_ - 1};
  }
  constexpr auto rbegin() requires(exposes_iterator) {
    return typename self_type::reverse_iterator{ptr_ + (size_ - 1)};
  }
  constexpr auto rend() requires(exposes_iterator) {
    return typename self_type::reverse_iterator{ptr_ - 1};
  }

  // Returns the number of elements viewed by the span.
  constexpr size_t size() const { return size_; }

  // Returns true if and only if the span contains no elements.
  constexpr bool empty() const { return size_ == 0; }

  // Returns a pointer to the data referenced by the span. Note that if the
  // span is empty this may still be non-null.
  constexpr std::add_const_t<viewed_type> *data() const { return ptr_; }
  constexpr viewed_type *data() requires(not std::is_const_v<viewed_type>) {
    return ptr_;
  }

  // Returns the projection of the first element. Behavior is undefined if the
  // span is empty.
  constexpr decltype(auto) front() const requires(exposes_const_iterator) {
    assert(not empty());
    return *cbegin();
  }
  constexpr decltype(auto) front() requires(exposes_iterator) {
    assert(not empty());
    return *begin();
  }

  // Returns the projection of the last element. Behavior is undefined if the
  // span is empty.
  constexpr decltype(auto) back() const requires(exposes_const_iterator) {
    assert(not empty());
    return *crbegin();
  }

  constexpr decltype(auto) back() requires(exposes_iterator) {
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

  constexpr decltype(auto) operator[](size_t n) const
      requires(exposes_const_iterator) {
    assert(n < size());
    return *(begin() + n);
  }

  constexpr decltype(auto) operator[](size_t n) requires(exposes_iterator){
    assert(n < size());
    return *(begin() + n);
  }

 private:
  viewed_type *ptr_ = nullptr;
  size_t size_      = 0;
};

}  // namespace nth

#endif  // NTH_UTILITY_PROJECTED_SPAN_H
