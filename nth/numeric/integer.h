#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <climits>
#include <concepts>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include "nth/debug/debug.h"
#include "nth/io/serialize/deserialize.h"
#include "nth/io/serialize/serialize.h"
#include "nth/meta/concepts.h"

namespace nth {

enum class sign : int8_t { negative = -1, zero = 0, positive = 1 };

struct integer {
  // Assumptions used in construction.
  static_assert(sizeof(uintptr_t) == sizeof(unsigned long long));
  static_assert(sizeof(uintptr_t) == sizeof(uint64_t));

  integer() : size_(0), data_{0, 0} {}
  integer(long long n);
  integer(unsigned long long n);
  integer(std::signed_integral auto n) : integer(static_cast<long long>(n)) {}
  integer(std::unsigned_integral auto n)
      : integer(static_cast<unsigned long long>(n)) {}
  integer(integer const &);
  integer(integer &&);

  // Constructs an `integer` from the sequence of `unsigned long long`s, where
  // each word in `words` is interpreted from least-to-most significant. Note
  // that words are cast up to `unsigned long long` regardless of how they are
  // specified, so even if one writes the initializer list as `{uint8_t{1},
  // uint8_t{1}}`, this will be interpreted as 18446744073709551617 (assuming
  // sizeof(unsigned long long) is 8), rather than 257.
  static integer from_words(std::initializer_list<unsigned long long> words);

  integer &operator=(integer const &);
  integer &operator=(integer &&);

  friend bool operator==(integer const &lhs, integer const &rhs);
  friend bool operator!=(integer const &lhs, integer const &rhs);
  friend bool operator<(integer const &lhs, integer const &rhs);
  friend bool operator<=(integer const &lhs, integer const &rhs);
  friend bool operator>(integer const &lhs, integer const &rhs);
  friend bool operator>=(integer const &lhs, integer const &rhs);

  friend integer operator+(integer const &lhs, integer const &rhs);

  integer &operator*=(unsigned long long rhs);
  integer &operator*=(std::signed_integral auto n) {
    if (n == 0 or zero(*this)) {
      return *this = 0;
    } else if (n < 0) {
      sign_ = 1 - sign_;
    }
    return *this *= static_cast<std::make_unsigned_t<decltype(n)>>(n);
  }

  friend integer operator*(integer const &lhs, unsigned long long rhs);
  friend integer operator*(integer &&lhs, unsigned long long rhs);

  template <nth::decays_to<integer> I>
  friend integer operator*(I &&lhs, std::signed_integral auto rhs) {
    switch (rhs) {
      case 0: return 0;
      case 1: return lhs;
      case -1: return -std::forward<I>(lhs);
    }
    auto result = std::forward<I>(lhs);
    if (rhs < 0) { result.sign_ = 1 - result.sign_; }
    result.SlowMul(static_cast<std::make_unsigned_t<decltype(rhs)>>(rhs));
    return std::forward<I>(result);
  }
  template <nth::decays_to<integer> I>
  friend integer operator*(std::signed_integral auto lhs, I &&rhs) {
    return std::forward<I>(rhs) * lhs;
  }

  template <nth::decays_to<integer> I>
  friend integer operator*(unsigned long long lhs, I &&rhs) {
    return std::forward<I>(rhs) * lhs;
  }

  integer &&operator-() &&;
  integer operator-() const & { return -integer(*this); }
  integer &operator++() &;
  integer operator++(int);

  friend sign sign(integer const &n);
  friend bool negative(integer const &n);
  friend bool positive(integer const &n);
  friend bool zero(integer const &n);

  template <std::integral N>
  explicit operator N() const {
    if constexpr (std::is_signed_v<N>) {
      if (negative(*this)) {
        return -static_cast<N>(data_[0]);
      } else {
        return static_cast<N>(data_[0]);
      }
    } else {
      return static_cast<N>(data_[0]);
    }
  }

  friend void NthPrint(auto &p, auto &, integer const &n) {
    if (n.size_ == 0) {
      p.write("0");
      return;
    }
    p.write(negative(n) ? "-0x" : "0x");
    std::unique_ptr<char[]> buffer(new char[16 * n.size_]);
    p.write(n.PrintUsingBuffer(std::span(buffer.get(), 16 * n.size_)));
  }

  template <typename H>
  friend H AbslHashValue(H h, integer const &n) {
    h = H::combine(std::move(h), n.sign_);
    return H::combine_contiguous(std::move(h), n.words().data(),
                                 n.words().size());
  }

  friend bool NthSerialize(auto &s, integer const &n) {
    if (not nth::io::serialize_fixed(s, static_cast<bool>(n.sign_)) or
        not nth::io::serialize_integer(s, n.size_)) {
      return false;
    }
    for (uintptr_t w : n.words()) {
      if (not nth::io::serialize_fixed(s, w)) { return false; }
    }
    return true;
  }

  friend bool NthDeserialize(auto &d, integer &n) {
    bool sig;
    uintptr_t size;
    if (not nth::io::deserialize_fixed(d, sig) or
        not nth::io::deserialize_integer(d, size)) {
      return false;
    }
    n.sign_ = sig;
    n.size_ = size;
    switch (size) {
      case 0:return true;
      case 1: return nth::io::deserialize_fixed(d, n.data_[0]);
      case 2:
        return nth::io::deserialize_fixed(d, n.data_[0]) and
               nth::io::deserialize_fixed(d, n.data_[1]);
      default: {
        uintptr_t *ptr = new uintptr_t[size];
        n.data_[0]       = reinterpret_cast<uintptr_t>(ptr);
        n.data_[1]       = size;
        for (size_t i = 0; i < size; ++i) {
          if (not nth::io::deserialize_fixed(d, *ptr++)) { return false; }
        }
        return true;
      } break;
    }
  }

 private:
  void ResizeTo(uint64_t n);

  void ReduceMagnitudeByOne();
  void IncreaseMagnitudeByOne();

  std::span<uint64_t const> words() const;
  std::span<uint64_t> words();

  std::string_view PrintUsingBuffer(std::span<char> buffer) const;

  void SlowMul(unsigned long long);

  uint64_t unchecked_capacity() const;
  uint64_t *&unchecked_data();
  uint64_t const *unchecked_data() const;

  uint64_t size_ : sizeof(uint64_t) * CHAR_BIT - 1;
  uint64_t sign_ : 1;
  uint64_t data_[2];
};

sign sign(integer const &n);
bool negative(integer const &n);
bool positive(integer const &n);
bool zero(integer const &n);

}  // namespace nth

#endif  // NTH_NUMERIC_INTEGER_H
