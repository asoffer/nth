#include "integer.h"

#include <algorithm>

namespace nth {
namespace {

// Adds `rhs` to `lhs` and returns whether the result wrapped.
bool AddWithCarry(internal_integer::word_type &lhs,
                  internal_integer::word_type rhs) {
  auto previous = lhs;
  lhs += rhs;
  return lhs < previous;
}

// Subtracts `rhs` from `lhs` and returns whether the result wrapped.
bool SubWithCarry(internal_integer::word_type &lhs,
                  internal_integer::word_type rhs) {
  auto previous = lhs;
  lhs -= rhs;
  return lhs > previous;
}

}  // namespace

void Integer::ExtendBy(size_t words) {
  data_.resize(data_.size() + words, negative() ? ~word_type{} : word_type{});
}

void Integer::ExtendTo(size_t words) {
  if (words <= data_.size()) { return; }
  data_.resize(words, negative() ? ~word_type{} : word_type{});
}

Integer &Integer::operator*=(Integer const &rhs) { return *this = *this * rhs; }

Integer &Integer::operator+=(Integer const &rhs) {
  ExtendTo(rhs.data_.size());
  word_type carry = 0;
  for (size_t i = 0; i < data_.size(); ++i) {
    carry = AddWithCarry(data_[i], carry);
    carry += AddWithCarry(data_[i], rhs.data_[i]);
  }
  return *this;
}

Integer &Integer::operator-=(Integer const &rhs) {
  ExtendTo(rhs.data_.size());
  word_type carry = 0;
  for (size_t i = 0; i < data_.size(); ++i) {
    carry = SubWithCarry(data_[i], carry);
    carry += SubWithCarry(data_[i], rhs.data_[i]);
  }
  return *this;
}

Integer &Integer::operator<<=(size_t n) {
  ExtendBy(1 + (n - 1) / BitsPerWord);
  word_type const relative_shift = n % BitsPerWord;
  word_type const low_bit_mask =
      (word_type{1} << (BitsPerWord - relative_shift)) - word_type{1};
  word_type const high_bit_mask = ~low_bit_mask;
  word_type carry               = 0;
  for (auto iter = data_.begin(); iter != data_.end(); ++iter) {
    word_type previous_carry = std::exchange(carry, *iter & high_bit_mask);
    *iter = ((*iter & low_bit_mask) << relative_shift | previous_carry);
  }
  return *this;
}

Integer &Integer::operator>>=(size_t ) { return *this; }

Integer operator*(Integer const &lhs, Integer const &rhs) {
  Integer result;
  for (size_t i = 0; i < lhs.data_.size(); ++i) {
    result += (rhs << (i * Integer::BitsPerWord));
  }
  return result;
}

Integer operator<<(Integer lhs, size_t rhs) { return lhs <<= rhs; }
Integer operator>>(Integer lhs, size_t rhs) { return lhs >>= rhs; }

bool operator<(Integer const &lhs, Integer const &rhs) {
  bool lhs_neg = lhs.negative();
  bool rhs_neg = rhs.negative();
  if (lhs_neg != rhs_neg) { return lhs_neg; }

  if (lhs.data_.size() != rhs.data_.size()) {
    return (lhs.data_.size() < rhs.data_.size()) ^ lhs_neg;
  }

  auto [l, r] = std::mismatch(lhs.data_.rbegin(), lhs.data_.rend(),
                              rhs.data_.rbegin(), rhs.data_.rend());
  if (l == lhs.data_.rend()) {
    return false;
  } else if (l == lhs.data_.rbegin()) {
    if (lhs_neg) {
      return (*l + Integer::TopWordBit()) < (*r + Integer::TopWordBit());
    } else {
      return *l < *r;
    }
  } else {
    return (*l < *r) ^ lhs_neg;
  }
}

}  // namespace nth
