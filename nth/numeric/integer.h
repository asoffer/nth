#ifndef NTH_NUMERIC_INTEGER_H
#define NTH_NUMERIC_INTEGER_H

#include <climits>
#include <concepts>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include "nth/debug/debug.h"
#include "nth/format/interpolate/string.h"
#include "nth/io/reader/reader.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/meta/concepts/core.h"

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

  template <interpolation_string>
  friend nth::trivial_formatter NthInterpolateFormatter(nth::type_tag<integer>) {
    return {};
  }

  friend void NthFormat(io::writer auto &w, auto &, integer const &n) {
    if (n.size_ == 0) {
      nth::io::write_text(w, std::string_view("0"));
      return;
    }
    nth::io::write_text(w, std::string_view(negative(n) ? "-0x" : "0x"));
    std::unique_ptr<char[]> buffer(new char[16 * n.size_]);
    w.write(n.PrintUsingBuffer(std::span(buffer.get(), 16 * n.size_)));
  }

  template <typename H>
  friend H AbslHashValue(H h, integer const &n) {
    h = H::combine(std::move(h), n.sign_);
    return H::combine_contiguous(std::move(h), n.words().data(),
                                 n.words().size());
  }

 private:
  void ResizeTo(uint64_t n);

  void ReduceMagnitudeByOne();
  void IncreaseMagnitudeByOne();

  std::span<uint64_t const> words() const;
  std::span<uint64_t> words();

  std::span<std::byte const> PrintUsingBuffer(std::span<char> buffer) const;

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
