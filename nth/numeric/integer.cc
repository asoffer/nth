#include "integer.h"
#include <iostream>

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ostream>

namespace nth {
namespace {

int MagnitudeCompare(std::span<uintptr_t const> lhs,
                     std::span<uintptr_t const> rhs) {

  if (lhs.size() < rhs.size()) { return 1; }
  if (lhs.size() > rhs.size()) { return -1; }
  auto [l, r] = std::mismatch(lhs.rbegin(), lhs.rend(), rhs.rbegin());
  if (l == lhs.rend()) { return 0; }
  assert(r != rhs.rend());
  return (*l < *r) ? 1 : -1;
}

// Adds `rhs` to `lhs` and returns whether the result wrapped.
bool AddWithCarry(uintptr_t &lhs, uintptr_t rhs) {
  auto previous = lhs;
  lhs += rhs;
  return lhs < previous;
}

// Subtracts `rhs` from `lhs` and returns whether the result wrapped.
bool SubWithCarry(uintptr_t &lhs, uintptr_t rhs) {
  auto previous = lhs;
  lhs -= rhs;
  return lhs > previous;
}

}  // namespace

Integer::Integer(Integer const &n) {
  std::memcpy(d_, n.d_, sizeof(uintptr_t) * 3);
  if (n.inlined()) {
    set_data(&d_[1]);
  } else {
    auto *ptr = new uintptr_t[n.size()];
    std::memcpy(ptr, n.data(), n.size() * sizeof(uintptr_t));
    set_data(ptr);
  }
}
Integer::Integer(Integer &&n) {
  std::memcpy(d_, n.d_, sizeof(uintptr_t) * 3);
  if (n.inlined()) {
    set_data(&d_[1]);
  } else {
    std::memset(n.d_, 0, sizeof(uintptr_t) * 3);
  }
}

Integer &Integer::operator=(Integer const &n) {
  std::memcpy(d_, n.d_, sizeof(uintptr_t) * 3);
  if (n.inlined()) {
    set_data(&d_[1]);
  } else {
    auto *ptr = new uintptr_t[n.size()];
    std::memcpy(ptr, n.data(), n.size());
    set_data(ptr);
  }

  return *this;
}

Integer &Integer::operator=(Integer &&n) {
  std::memcpy(d_, n.d_, sizeof(uintptr_t) * 3);
  if (n.inlined()) {
    set_data(&d_[1]);
  } else {
    std::memset(n.d_, 0, sizeof(uintptr_t) * 3);
  }

  return *this;
}

uintptr_t const *Integer::ssb_begin() const {
  assert(inlined());
  return &d_[1];
}

uintptr_t const *Integer::ssb_end() const {
  assert(inlined());
  return &d_[3];
}

uintptr_t *Integer::ssb_begin() {
  assert(inlined());
  return &d_[1];
}
uintptr_t *Integer::ssb_end() {
  assert(inlined());
  return &d_[3];
}

uintptr_t Integer::nonssb_size() const {
  assert(not inlined());
  return d_[1];
}
uintptr_t Integer::nonssb_capacity() const {
  assert(not inlined());
  return d_[0];
}

uintptr_t *Integer::data() {
  return reinterpret_cast<uintptr_t *>(d_[0] & ~AllFlags);
}
uintptr_t const *Integer::data() const {
  return reinterpret_cast<uintptr_t const *>(d_[0] & ~AllFlags);
}

void Integer::set_data(uintptr_t *ptr) {
  d_[0] = reinterpret_cast<uintptr_t>(ptr) | (d_[0] & AllFlags);
}

void Integer::set_nonssb_size(uintptr_t n) {
  assert(not inlined());
  d_[1] = n;
}

void Integer::set_nonssb_capacity(uintptr_t n) {
  assert(not inlined());
  d_[2] = n;
}

std::span<uintptr_t> Integer::span() {
  auto *ptr = data();
  return std::span<uintptr_t>(ptr, ptr + size());
}

std::span<uintptr_t const> Integer::span() const {
  auto const *ptr = data();
  return std::span<uintptr_t const>(ptr, ptr + size());
}

Integer Integer::operator-() const {
  Integer result = *this;
  result.negate();
  return result;
}

void Integer::negate() { set_negative_flag(positive()); }

Integer &Integer::operator+=(Integer const &rhs) {
  if (rhs.is_zero()) { return *this; }
  if (is_zero()) { return *this = rhs; }
  if (negative() == rhs.negative()) {
    MagnitudeAddImpl(rhs.span());
  } else {
    MagnitudeSubImpl(rhs.span());
  }
  return *this;
}

Integer &Integer::operator-=(Integer const &rhs) {
  if (rhs.is_zero()) { return *this; }
  if (is_zero()) { return *this = -rhs; }
  if (negative() == rhs.negative()) {
    MagnitudeSubImpl(rhs.span());
  } else {
    MagnitudeAddImpl(rhs.span());
  }
  return *this;
}

void Integer::MagnitudeAddImpl(std::span<uintptr_t const> rhs) {
  ZeroExtendInWords(rhs.size());
  std::span s = span();
  auto l_iter = s.begin();
  auto r_iter = rhs.begin();
  uintptr_t carry = 0;
  for (; l_iter != s.end(); ++l_iter, ++r_iter) {
    carry = AddWithCarry(*l_iter, carry);
    carry += AddWithCarry(*l_iter, *r_iter);
  }
  if (carry) {
    ZeroExtendInWords(size() + uintptr_t{1});
    *span().rbegin() = carry;
  }
}

void Integer::MagnitudeSubImpl(std::span<uintptr_t const> rhs) {
  ZeroExtendInWords(rhs.size());
  std::span s     = span();
  auto l_iter     = s.begin();
  auto r_iter     = rhs.begin();
  uintptr_t carry = 0;
  for (; l_iter != s.end(); ++l_iter, ++r_iter) {
    carry = SubWithCarry(*l_iter, carry);
    carry += SubWithCarry(*l_iter, *r_iter);
  }
  if (carry) {
    ZeroExtendInWords(size() - uintptr_t{1});
    *span().rbegin() = carry;
  }
}

Integer operator+(Integer const &lhs, Integer const &rhs) {
  if (lhs.size() > rhs.size()) {
    Integer copy = lhs;
    return copy += rhs;
  } else {
    Integer copy = rhs;
    return copy += lhs;
  }
}

Integer operator+(Integer &&lhs, Integer const &rhs) { return lhs += rhs; }
Integer operator+(Integer const &lhs, Integer &&rhs) { return rhs += lhs; }
Integer operator+(Integer &&lhs, Integer &&rhs) { return lhs += rhs; }

Integer operator-(Integer const &lhs, Integer const &rhs) {
  if (lhs.size() > rhs.size()) {
    Integer copy = lhs;
    return copy -= rhs;
  } else {
    Integer copy = rhs;
    return copy -= lhs;
  }
}

Integer operator-(Integer &&lhs, Integer const &rhs) { return lhs -= rhs; }
Integer operator-(Integer const &lhs, Integer &&rhs) { return rhs -= lhs; }
Integer operator-(Integer &&lhs, Integer &&rhs) { return lhs -= rhs; }

Integer &Integer::operator<<=(size_t n) {
  if (is_zero()) { return *this; }

  size_t pre_bits_needed  = BitsNeededToRepresent();
  size_t post_bits_needed = pre_bits_needed + n;

  size_t post_words_needed =
      (((post_bits_needed - 1) | (BitsPerWord - 1)) + 1) / BitsPerWord;
  size_t pre_words_needed =
      (((pre_bits_needed - 1) | (BitsPerWord - 1)) + 1) / BitsPerWord;
  ZeroExtendInWords(post_words_needed);

  uintptr_t const relative_shift = n % BitsPerWord;

  auto const high_part = [&](uintptr_t word) {
    if (relative_shift == 0) { return word; }
    return word >> (BitsPerWord - relative_shift);
  };
  auto const low_part = [&](uintptr_t word) {
    if (relative_shift == 0) { return uintptr_t{0}; }

    return word << relative_shift;
  };

  size_t extended_words = post_words_needed - pre_words_needed;

  std::span s    = span();
  auto from_iter = s.rbegin() + extended_words;
  auto to_iter   = s.rbegin();
  assert(from_iter != s.rend());

  if (high_part(*from_iter) == 0) {
    *to_iter = low_part(*from_iter);
    ++from_iter;
  }

  for (; from_iter != s.rend(); ++to_iter, ++from_iter) {
    *to_iter       = high_part(*from_iter);
    *(to_iter + 1) = low_part(*from_iter);                                 
  }
  ++to_iter;
  for (; to_iter != s.rend(); ++to_iter) { *to_iter = 0; }

  return *this;
}

void Integer::ZeroExtendInWords(size_t w) {
  if (capacity() >= w) {
    if (capacity() > ssb_size()) { set_nonssb_size(w); }
  } else {
    size_t n  = size();
    auto *ptr = new uintptr_t[w];
    std::memcpy(ptr, data(), sizeof(uintptr_t) * n);
    std::memset(ptr + n, 0, sizeof(uintptr_t) * (w - n));
    set_data(ptr);
    set_ssb_flag(false);
    set_nonssb_size(w);
    set_nonssb_capacity(w);
  }
}

Integer &Integer::operator>>=(size_t) { return *this; }

Integer operator<<(Integer lhs, size_t rhs) { return lhs <<= rhs; }
Integer operator>>(Integer lhs, size_t rhs) { return lhs >>= rhs; }

bool operator==(Integer const &lhs, Integer const &rhs) {
  return lhs.negative() == rhs.negative() and
         MagnitudeCompare(lhs.span(), rhs.span()) == 0;
}

bool operator<(Integer const &lhs, Integer const &rhs) {
  bool lhs_neg = lhs.negative();
  bool rhs_neg = rhs.negative();
  if (lhs_neg != rhs_neg) { return lhs_neg; }
  int result = MagnitudeCompare(lhs.span(), rhs.span());
  return result != 0 and (result == 1) xor lhs_neg;
}

void PrintTo(Integer const &n, std::ostream *os) {
  if (n.is_zero()) {
    *os << "0";
    return;
  }
  *os << (n.positive() ? "0x" : "-0x");
  auto s = n.span();
  std::ios_base::fmtflags f(os->flags());
  *os << std::hex << std::setfill('0') << std::setw(sizeof(uintptr_t) * 2);
  for (auto iter = s.rbegin(); iter != s.rend(); ++iter) { *os << *iter; }
  os->flags(f);
}

Integer Integer::FromHex(std::string_view sv) {
  Integer n;
  // TODO: Improve this horribly slow implementation.
  for (unsigned char c : sv) {
    if ('a' <= c and c <= 'f') {
      c = (c - 'a') + 10;
    } else if ('A' <= c and c <= 'F') {
      c = (c - 'A') + 10;
    } else if ('0' <= c and c <= '9') {
      c = c - '0';
    } else {
      assert(false && "unreachable");
    }
  }
  return n;
}

}  // namespace nth
