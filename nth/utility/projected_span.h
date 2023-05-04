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
template <typename T>
struct reverse_ptr {
  constexpr reverse_ptr(T *ptr = nullptr) : ptr_(ptr) {}

  constexpr T &operator*() const { return *ptr_; }
  constexpr reverse_ptr &operator++() {
    --ptr_;
    return *this;
  }
  constexpr reverse_ptr operator++(int) { return reverse_ptr{ptr_--}; }
  constexpr reverse_ptr &operator+=(ptrdiff_t rhs) {
    ptr_ -= rhs;
    return *this;
  }

  friend constexpr reverse_ptr operator+(reverse_ptr lhs, ptrdiff_t rhs) {
    return reverse_ptr{lhs.ptr_ - rhs};
  }
  friend constexpr reverse_ptr operator+(ptrdiff_t lhs, reverse_ptr rhs) {
    return rhs + lhs;
  }

  constexpr reverse_ptr &operator--() {
    ++ptr_;
    return *this;
  }
  constexpr reverse_ptr operator--(int) { return reverse_ptr{ptr_++}; }
  constexpr reverse_ptr &operator-=(ptrdiff_t rhs) {
    ptr_ += rhs;
    return *this;
  }
  friend constexpr reverse_ptr operator-(reverse_ptr lhs, ptrdiff_t rhs) {
    return reverse_ptr{lhs.ptr_ + rhs};
  }
  friend constexpr ptrdiff_t operator-(reverse_ptr lhs, reverse_ptr rhs) {
    return rhs.ptr_ - lhs.ptr_;
  }
  constexpr decltype(auto) operator[](ptrdiff_t rhs) const {
    return *(*this + rhs);
  }

  friend constexpr std::strong_ordering operator<=>(reverse_ptr lhs,
                                                    reverse_ptr rhs) {
    return rhs.ptr_ <=> lhs.ptr_;
  }
  friend constexpr bool operator==(reverse_ptr lhs, reverse_ptr rhs) = default;
  friend constexpr bool operator!=(reverse_ptr lhs, reverse_ptr rhs) = default;

 private:
  T *ptr_;
};

template <typename T, auto Projection>
struct ConditionallyExposeIterator {};

template <typename T, auto Projection>
requires(not std::is_const_v<T> and not requires {
  Projection(std::declval<std::add_lvalue_reference_t<std::add_const_t<T>>>());
}) struct ConditionallyExposeIterator<T, Projection> {
 private:
  using value_type =
      decltype(Projection(std::declval<std::add_lvalue_reference_t<T>>()));

 public:
  // Iterator over the spanned elements. Invoking `operator*` on this iterator
  // invokes `Projection` on a reference to the underlying value referred to
  // by the iterator.
  struct iterator {
    using difference_type = ptrdiff_t;
    using value_type      = value_type;

    iterator() = default;

    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr iterator &operator++() {
      ++ptr_;
      return *this;
    }
    constexpr iterator operator++(int) { return iterator{ptr_++}; }
    constexpr iterator &operator--() {
      --ptr_;
      return *this;
    }
    constexpr iterator operator--(int) { return iterator{ptr_--}; }
    constexpr iterator &operator+=(difference_type rhs) {
      ptr_ += rhs;
      return *this;
    }
    constexpr iterator &operator-=(difference_type rhs) {
      ptr_ -= rhs;
      return *this;
    }
    friend constexpr iterator operator+(iterator lhs, difference_type rhs) {
      return iterator{lhs.ptr_ + rhs};
    }
    friend constexpr iterator operator+(difference_type lhs, iterator rhs) {
      return rhs + lhs;
    }
    friend constexpr iterator operator-(iterator lhs, difference_type rhs) {
      return iterator{lhs.ptr_ - rhs};
    }
    friend constexpr difference_type operator-(iterator lhs, iterator rhs) {
      return lhs.ptr_ - rhs.ptr_;
    }
    constexpr decltype(auto) operator[](difference_type rhs) const {
      return *(*this + rhs);
    }

    friend constexpr std::strong_ordering operator<=>(iterator,
                                                      iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit iterator(T *ptr) : ptr_(ptr) {}
    T *ptr_ = nullptr;
  };

  // Reverse iterator over the spanned elements. Invoking `operator*` on this
  // iterator invokes `Projection` on a reference to the underlying value
  // referred to by the iterator.
  struct reverse_iterator {
    using difference_type = ptrdiff_t;
    using value_type      = value_type;

    reverse_iterator() = default;

    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr reverse_iterator &operator++() {
      ++ptr_;
      return *this;
    }
    constexpr reverse_iterator operator++(int) {
      return reverse_iterator{ptr_++};
    }
    constexpr reverse_iterator &operator+=(difference_type rhs) {
      ptr_ += rhs;
      return *this;
    }
    friend constexpr reverse_iterator operator+(reverse_iterator lhs,
                                                difference_type rhs) {
      return reverse_iterator{lhs.ptr_ + rhs};
    }
    friend constexpr reverse_iterator operator+(difference_type lhs,
                                                reverse_iterator rhs) {
      return rhs + lhs;
    }
    constexpr reverse_iterator &operator--() {
      --ptr_;
      return *this;
    }
    constexpr reverse_iterator operator--(int) {
      return reverse_iterator{ptr_--};
    }
    constexpr reverse_iterator &operator-=(difference_type rhs) {
      ptr_ -= rhs;
      return *this;
    }
    friend constexpr reverse_iterator operator-(reverse_iterator lhs,
                                                difference_type rhs) {
      return reverse_iterator{lhs.ptr_ - rhs};
    }

    friend constexpr difference_type operator-(reverse_iterator lhs,
                                               reverse_iterator rhs) {
      return lhs.ptr_ - rhs.ptr_;
    }
    constexpr decltype(auto) operator[](difference_type rhs) const {
      return *(*this + rhs);
    }
    friend constexpr std::strong_ordering operator<=>(
        reverse_iterator, reverse_iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit reverse_iterator(T *ptr) : ptr_(ptr) {}
    constexpr explicit reverse_iterator(reverse_ptr<T> ptr) : ptr_(ptr) {}
    reverse_ptr<T> ptr_ = nullptr;
  };
};

template <typename, auto>
struct ConditionallyExposeConstIterator {};

template <typename T, auto Projection>
requires(requires {
  Projection(std::declval<std::add_lvalue_reference_t<std::add_const_t<T>>>());
})  //
    struct ConditionallyExposeConstIterator<T, Projection> {
 private:
  using value_type = decltype(Projection(
      std::declval<std::add_lvalue_reference_t<std::add_const_t<T>>>()));

 public:
  // Iterator over the spanned elements. Invoking `operator*` on this iterator
  // invokes `Projection` on a reference to the underlying value referred to by
  // the iterator.
  struct const_iterator {
    using difference_type = ptrdiff_t;
    using value_type      = value_type;

    const_iterator() = default;

    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr const_iterator &operator++() {
      ++ptr_;
      return *this;
    }
    constexpr const_iterator operator++(int) { return const_iterator{ptr_++}; }
    constexpr const_iterator &operator--() {
      --ptr_;
      return *this;
    }
    constexpr const_iterator operator--(int) { return const_iterator{ptr_--}; }
    friend constexpr const_iterator operator+(const_iterator lhs,
                                              difference_type rhs) {
      return const_iterator{lhs.ptr_ + rhs};
    }
    friend constexpr const_iterator operator+(difference_type lhs,
                                              const_iterator rhs) {
      return rhs + lhs;
    }
    constexpr const_iterator &operator+=(difference_type rhs) {
      ptr_ += rhs;
      return *this;
    }
    constexpr const_iterator &operator-=(difference_type rhs) {
      ptr_ -= rhs;
      return *this;
    }
    friend constexpr const_iterator operator-(const_iterator lhs,
                                              difference_type rhs) {
      return const_iterator{lhs.ptr_ - rhs};
    }
    friend constexpr difference_type operator-(const_iterator lhs,
                                               const_iterator rhs) {
      return lhs.ptr_ - rhs.ptr_;
    }
    constexpr decltype(auto) operator[](difference_type rhs) const {
      return *(*this + rhs);
    }

    friend constexpr std::strong_ordering operator<=>(const_iterator,
                                                      const_iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit const_iterator(T *ptr) : ptr_(ptr) {}
    T *ptr_ = nullptr;
  };

  // Reverse iterator over the spanned elements. Invoking `operator*` on this
  // iterator invokes `Projection` on a reference to the underlying value
  // referred to by the iterator.
  struct const_reverse_iterator {
    using difference_type = ptrdiff_t;
    using value_type      = value_type;

    const_reverse_iterator() = default;

    constexpr decltype(auto) operator*() const { return Projection(*ptr_); }
    constexpr const_reverse_iterator &operator++() {
      ++ptr_;
      return *this;
    }
    constexpr const_reverse_iterator operator++(int) {
      return const_reverse_iterator{ptr_++};
    }
    constexpr const_reverse_iterator &operator--() {
      --ptr_;
      return *this;
    }
    constexpr const_reverse_iterator operator--(int) {
      return const_reverse_iterator{ptr_--};
    }
    friend constexpr const_reverse_iterator operator+(
        const_reverse_iterator lhs, difference_type rhs) {
      return const_reverse_iterator{lhs.ptr_ + rhs};
    }
    friend constexpr const_reverse_iterator operator+(
        difference_type lhs, const_reverse_iterator rhs) {
      return rhs + lhs;
    }
    constexpr const_reverse_iterator &operator+=(difference_type rhs) {
      ptr_ += rhs;
      return *this;
    }
    constexpr const_reverse_iterator &operator-=(difference_type rhs) {
      ptr_ -= rhs;
      return *this;
    }
    friend constexpr const_reverse_iterator operator-(
        const_reverse_iterator lhs, difference_type rhs) {
      return const_reverse_iterator{lhs.ptr_ - rhs};
    }
    friend constexpr difference_type operator-(const_reverse_iterator lhs,
                                               const_reverse_iterator rhs) {
      return lhs.ptr_ - rhs.ptr_;
    }
    constexpr decltype(auto) operator[](difference_type rhs) const {
      return *(*this + rhs);
    }

    friend constexpr std::strong_ordering operator<=>(
        const_reverse_iterator, const_reverse_iterator) = default;

   private:
    friend struct ProjectedSpan<T, Projection>;
    constexpr explicit const_reverse_iterator(T *ptr) : ptr_(ptr) {}
    constexpr explicit const_reverse_iterator(reverse_ptr<T> ptr) : ptr_(ptr) {}
    reverse_ptr<T> ptr_ = nullptr;
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
  using self_type = ProjectedSpan<T, Projection>;

  static constexpr bool exposes_iterator =
      not std::is_const_v<T> and not requires {
    Projection(
        std::declval<std::add_lvalue_reference_t<std::add_const_t<T>>>());
  };
  static constexpr bool exposes_const_iterator = requires {
    Projection(
        std::declval<std::add_lvalue_reference_t<std::add_const_t<T>>>());
  };
  static_assert(exposes_iterator or exposes_const_iterator);

 public:
  using viewed_type = T;
  using value_type =
      decltype(Projection(std::declval<std::add_lvalue_reference_t<T>>()));

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

  constexpr decltype(auto) operator[](size_t n) requires(exposes_iterator) {
    assert(n < size());
    return *(begin() + n);
  }

 private:
  viewed_type *ptr_ = nullptr;
  size_t size_      = 0;
};

}  // namespace nth

#endif  // NTH_UTILITY_PROJECTED_SPAN_H
