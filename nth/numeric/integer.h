#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <concepts>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <span>
#include <sstream>

namespace nth {

enum class sign : int8_t { negative = -1, zero = 0, positive = 1 };

struct integer {
  integer() : size_(0), data_{0, 0} {}
  integer(long long n);
  integer(unsigned long long n);
  integer(std::signed_integral auto n) : integer(long_long{n}) {}
  integer(std::unsigned_integral auto n) : integer(unsigned_long_long{n}) {}
  integer(integer const &);
  integer(integer &&);

  // Constructs an `integer` from the sequence of `uintptr_t`s.
  static integer from_words(std::span<uintptr_t const> words);

  integer &operator=(integer const &);
  integer &operator=(integer &&);

  friend bool operator==(integer const &lhs, integer const &rhs);
  friend bool operator!=(integer const &lhs, integer const &rhs);
  friend bool operator<(integer const &lhs, integer const &rhs);
  friend bool operator<=(integer const &lhs, integer const &rhs);
  friend bool operator>(integer const &lhs, integer const &rhs);
  friend bool operator>=(integer const &lhs, integer const &rhs);

  friend integer operator+(integer const &lhs, integer const &rhs);

  integer &&operator-() &&;
  integer operator-() const & { return -integer(*this); }
  integer &operator++() &;
  integer operator++(int);

  friend sign sign(integer const &n);
  friend bool negative(integer const &n);
  friend bool positive(integer const &n);
  friend bool zero(integer const &n);

  friend void NthPrint(auto &p, auto &, integer const &n) {
    if (n.size_ == 0) {
      p.write("0");
      return;
    }
    std::span w = n.words();
    std::stringstream ss;
    auto iter = w.rbegin();
    if (negative(n)) {
      ss << "-0x" << std::hex << *iter++;
      for (; iter != w.rend(); ++iter) {
        ss << std::setfill('0') << std::setw(sizeof(uintptr_t) * 2) << *iter;
      }
    } else {
      ss << "0x" << std::hex << *iter++;
      for (; iter != w.rend(); ++iter) {
        ss << std::setfill('0') << std::setw(sizeof(uintptr_t) * 2) << *iter;
      }
    }
    p.write(ss.str());
  }

 private:
  using long_long          = long long;
  using unsigned_long_long = unsigned long long;

  void ResizeTo(uintptr_t n);

  void ReduceMagnitudeByOne();
  void IncreaseMagnitudeByOne();

  std::span<uintptr_t const> words() const;
  std::span<uintptr_t> words();

  uintptr_t unchecked_capacity() const;
  uintptr_t *&unchecked_data();
  uintptr_t const *unchecked_data() const;

  uintptr_t size_ : sizeof(uintptr_t) * CHAR_BIT - 1;
  uintptr_t sign_ : 1;
  uintptr_t data_[2];
};

sign sign(integer const &n);
bool negative(integer const &n);
bool positive(integer const &n);
bool zero(integer const &n);

}  // namespace nth

#endif  // NTH_NUMERIC_INTEGER_H
