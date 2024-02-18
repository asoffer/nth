#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <climits>
#include <concepts>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

namespace nth {

enum class sign : int8_t { negative = -1, zero = 0, positive = 1 };

struct integer {
  // Assumptions used in construction.
  static_assert(sizeof(uintptr_t) == sizeof(long long));
  static_assert(sizeof(uintptr_t) == sizeof(unsigned long long));

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
    p.write(negative(n) ? "-0x" : "0x");
    std::unique_ptr<char[]> buffer(new char[16 * n.size_]);
    p.write(n.PrintUsingBuffer(std::span(buffer.get(), 16 * n.size_)));
  }

 private:
  void ResizeTo(uintptr_t n);

  void ReduceMagnitudeByOne();
  void IncreaseMagnitudeByOne();

  std::span<uintptr_t const> words() const;
  std::span<uintptr_t> words();

  std::string_view PrintUsingBuffer(std::span<char> buffer) const;

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
