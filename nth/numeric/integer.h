#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <bit>
#include <climits>
#include <cstdint>
#include <cstring>
#include <iosfwd>
#include <span>
#include <string_view>
#include <type_traits>

namespace nth {

struct Integer {
  Integer() : d_{reinterpret_cast<uintptr_t>(&d_[1]) | SsbFlag, 0, 0} {}
  Integer(std::integral auto n) : Integer() {
    static_assert(sizeof(n) <= sizeof(uintptr_t));
    if (n == 0) { return; }
    if constexpr (std::is_unsigned_v<decltype(n)>) {
      auto iter = ssb_begin();
      *iter     = n;
      *++iter   = 0;
    } else {
      // NOTE: Relies on two-complement representation.
      intptr_t in = n;
      auto iter   = ssb_begin();
      std::memcpy(iter, &in, sizeof(*iter));
      *(iter + 1) = 0;
      if (n < 0) {
        set_negative_flag(true);
        *iter = ~*iter + 1;
      }
    }
  }

  Integer(Integer const &);
  Integer(Integer &&);
  Integer &operator=(Integer const &);
  Integer &operator=(Integer &&);

  static Integer FromHex(std::string_view sv);

  template <typename H>
  friend H AbslHashValue(H h, Integer const &n) {
    return H::combine(std::move(h), n.span());
  }

  friend Integer operator+(Integer const &lhs, Integer const &rhs);
  friend Integer operator+(Integer &&lhs, Integer const &rhs);
  friend Integer operator+(Integer const &lhs, Integer &&rhs);
  friend Integer operator+(Integer &&lhs, Integer &&rhs);

  friend Integer operator-(Integer const &lhs, Integer const &rhs);
  friend Integer operator-(Integer &&lhs, Integer const &rhs);
  friend Integer operator-(Integer const &lhs, Integer &&rhs);
  friend Integer operator-(Integer &&lhs, Integer &&rhs);

  friend Integer operator*(Integer const &lhs, Integer const &rhs);
  friend Integer operator/(Integer const &lhs, Integer const &rhs);
  friend Integer operator%(Integer const &lhs, Integer const &rhs);

  Integer &operator+=(Integer const &rhs);
  Integer &operator-=(Integer const &rhs);
  Integer &operator*=(Integer const &rhs);
  Integer &operator/=(Integer const &rhs);
  Integer &operator%=(Integer const &rhs);

  Integer operator-() const;
  void negate();

  Integer &operator<<=(size_t rhs);
  Integer &operator>>=(size_t rhs);
  friend Integer operator<<(Integer lhs, size_t rhs);
  friend Integer operator>>(Integer lhs, size_t rhs);

  friend bool operator==(Integer const &lhs, Integer const &rhs);
  friend bool operator!=(Integer const &lhs, Integer const &rhs) {
    return not(lhs == rhs);
  }
  friend bool operator<(Integer const &lhs, Integer const &rhs);
  friend bool operator>(Integer const &lhs, Integer const &rhs) {
    return rhs < lhs;
  }
  friend bool operator<=(Integer const &lhs, Integer const &rhs) {
    return not(rhs < lhs);
  }
  friend bool operator>=(Integer const &lhs, Integer const &rhs) {
    return not(lhs < rhs);
  }

  // Do not call directly. For use by GoogleTest only.
  friend void PrintTo(Integer const &n, std::ostream *os);

  // Returns a `std::span` over the `uintptr_t` words order from least- to
  // most-significant.
  std::span<uintptr_t const> span() const;

 private:
  static_assert(alignof(uintptr_t) > 4);
  static_assert(std::is_same_v<uintptr_t, uint64_t>);

  static constexpr uintptr_t NegativeFlag = 0b01;
  static constexpr uintptr_t SsbFlag      = 0b10;
  static constexpr uintptr_t AllFlags     = 0b11;
  static constexpr size_t BitsPerWord     = sizeof(uintptr_t) * CHAR_BIT;
  static constexpr size_t SmallBufferBits = 2 * BitsPerWord;

  // Return true if the number is positive and false otherwise.
  bool positive() const {
    return (not is_zero()) and ((d_[0] & NegativeFlag) == 0);
  }
  // Return true if the number is negative and false otherwise.
  bool negative() const { return (d_[0] & NegativeFlag) == 1; }
  bool is_zero() const { return d_[1] == 0 and d_[2] == 0; }

  uintptr_t *data();
  uintptr_t const *data() const;
  void set_data(uintptr_t *ptr);

  // Returns a pointer to the beginning of the small-size buffer. Requires that
  // the stored value be represented within the small-size buffer.
  uintptr_t const *ssb_begin() const;
  uintptr_t *ssb_begin();

  // Returns a pointer to the end of the small-size buffer. Requires that
  // the stored value be represented within the small-size buffer.
  uintptr_t const *ssb_end() const;
  uintptr_t *ssb_end();

  // Returns the size of the stored data under the assumption that the
  // small-size buffer is not being used.
  uintptr_t nonssb_size() const;

  size_t ssb_size() const { return 2; }

  // Returns the capacity of the buffer under the assumption that the small-size
  // buffer is not being used.
  uintptr_t nonssb_capacity() const;

  // Returns whether the small-size buffer is being used for storage.
  bool inlined() const { return d_[0] & SsbFlag; }

  size_t size() const { return inlined() ? 2 : nonssb_size(); }
  void set_nonssb_size(uintptr_t n);

  size_t capacity() const { return inlined() ? 2 : nonssb_capacity(); }
  void set_nonssb_capacity(uintptr_t n);

  std::span<uintptr_t> span();

  void set_negative_flag(bool b) {
    d_[0] &= ~NegativeFlag;
    d_[0] |= b ? NegativeFlag : 0;
  }
  void set_ssb_flag(bool b) {
    d_[0] &= ~SsbFlag;
    d_[0] |= b ? SsbFlag : 0;
  }

  size_t BitsNeededToRepresent() const {
    if (inlined()) {
      if (d_[2] == 0) {
        return BitsPerWord - std::countl_zero(d_[1]);
      } else {
        return 2 * BitsPerWord - std::countl_zero(d_[2]);
      }
    } else {
      return BitsPerWord * nonssb_size() - std::countl_zero(span().back() - 1);
    }
  }

  // Ensures there is enough capacity for `w` words, possibly resizing the
  // buffer and copying it's contents if required. Sets the size to the
  // capacity.
  void ZeroExtendInWords(size_t w);

  void MagnitudeAddImpl(std::span<uintptr_t const> rhs);
  void MagnitudeSubImpl(std::span<uintptr_t const> rhs);

  // There are three valid representations formats for `d_`.
  // If `d_[0] == 0`, the represented number is zero. Otherwise, the least
  // significant bit of `d_[0]` is used to indicate whether or not the number is
  // positive (set means negative, unset means positive). The
  // second-least-significant bit represents whether the data is stored inline
  // in the small-size-buffer or on the heap (set means stored inline, unset
  // means on the heap). If the data is stored inline, then `d_[1]` and
  // `d_[1]` represent the value (with `d_[1]` representing the least
  // significant `uintptr_t` and `d_[2]` representing the most significant
  // `uintptr_t`. Otherwise, `d_[0]` points to the heap at an allocation of size
  // `d_[2]` (called the "capacity"), but only the first `d_[1]` (called the
  // "size") `uintptr_t`s of which are used to represent the data.
  uintptr_t d_[3] = {0, 0, 0};
};

}  // namespace nth

#endif  // NTH_NUMERIC_INTEGER_H
