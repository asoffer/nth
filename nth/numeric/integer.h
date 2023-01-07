#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <climits>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

namespace nth {
namespace internal_integer {
using word_type = uintmax_t;
}  // namespace internal_integer

struct Integer {
  Integer() = default;

  Integer(std::integral auto n) {
    if (n == 0) { return; }
    if constexpr (std::is_unsigned_v<decltype(n)>) {
      data_.push_back(n);
    } else {
      if (n < 0) {
        intmax_t copy = n;
        uintmax_t as_unsigned;
        static_assert(sizeof(copy) == sizeof(as_unsigned));
        std::memcpy(&as_unsigned, &copy, sizeof(copy));
        data_.push_back(as_unsigned);
      } else {
        data_.push_back(n);
      }
    }
  }

  Integer(Integer const &)            = default;
  Integer(Integer &&)                 = default;
  Integer &operator=(Integer const &) = default;
  Integer &operator=(Integer &&)      = default;

  template <typename H>
  friend H AbslHashValue(H h, Integer const &n) {
    return H::combine(std::move(h), n.data_);
  }

  friend Integer operator+(Integer const &lhs, Integer const &rhs);
  friend Integer operator-(Integer const &lhs, Integer const &rhs);
  friend Integer operator*(Integer const &lhs, Integer const &rhs);
  friend Integer operator/(Integer const &lhs, Integer const &rhs);
  friend Integer operator%(Integer const &lhs, Integer const &rhs);

  Integer operator-() const;

  Integer &operator+=(Integer const &rhs);
  Integer &operator-=(Integer const &rhs);
  Integer &operator*=(Integer const &rhs);
  Integer &operator/=(Integer const &rhs);
  Integer &operator%=(Integer const &rhs);

  Integer &operator<<=(size_t rhs);
  Integer &operator>>=(size_t rhs);
  friend Integer operator<<(Integer lhs, size_t rhs);
  friend Integer operator>>(Integer lhs, size_t rhs);

  friend bool operator==(Integer const &lhs, Integer const &rhs) {
    return lhs.data_ == rhs.data_;
  }
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

 private:
  using word_type                      = internal_integer::word_type;
  static constexpr size_t BytesPerWord = sizeof(word_type);
  static constexpr size_t BitsPerWord  = BytesPerWord * CHAR_BIT;

  // Returns the number of bytes currently used to represent the given number.
  size_t bytes() const { return data_.size() * BytesPerWord; }

  // Return true if the number is positive and false otherwise.
  bool positive() const {
    return not data_.empty() and not TopWordBitIsSet(*data_.rbegin());
  }
  // Return true if the number is negative and false otherwise.
  bool negative() const {
    return not data_.empty() and TopWordBitIsSet(*data_.rbegin());
  }

  static constexpr word_type TopWordBit() {
    return word_type{1} << (sizeof(word_type) * CHAR_BIT - 1);
  }
  static constexpr bool TopWordBitIsSet(word_type n) {
    return (n & TopWordBit()) != 0;
  }

  void ExtendBy(size_t words);
  void ExtendTo(size_t words);

  std::basic_string<word_type> data_;
};

}  // namespace nth

#endif  // NTH_NUMERIC_INTEGER_H
