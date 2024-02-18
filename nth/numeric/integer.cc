#include "nth/numeric/integer.h"

#include <cstring>
#include <limits>

#include "nth/debug/debug.h"

namespace nth {
namespace {

bool AddWord(std::span<uintptr_t> words, uintptr_t carry) {
  for (uintptr_t &word : words) {
    uintptr_t old = word;
    word += carry;
    if (word >= old) { return false; }
    carry = 1;
  }
  return true;
}

// Subtracts `borrow` from `words`, and returns a pointer just passed the last
// word in `words` which is non-zero.
uintptr_t *SubWord(std::span<uintptr_t> words, uintptr_t borrow) {
  NTH_REQUIRE((v.debug), words.empty() or words.back() != 0u);
  uintptr_t *last = words.data();
  uintptr_t *end  = words.data() + words.size();
  for (uintptr_t *ptr = words.data(); ptr != end; ++ptr) {
    if (*ptr > borrow) {
      *ptr -= borrow;
      return end;
    } else if (*ptr == borrow) {
      *ptr = 0;
      if (ptr + 1 == end) {
        return last;
      } else {
        return end;
      }
    } else {
      *ptr -= borrow;
      last   = ptr;
      borrow = 1;
    }
  }
  return end;
}

// Adds `words` to `out`, and returns a bool indicating whether an extra carry
// bit was left over after the addition. Assumes `words.size() <= out.size()`.
bool AddWords(std::span<uintptr_t const> words, std::span<uintptr_t> out) {
  auto out_iter   = out.begin();
  uintptr_t carry = 0;
  for (uintptr_t word : words) {
    uintptr_t old = *out_iter;
    *out_iter += word + carry;
    if (word + carry != 0) { carry = (*out_iter < old) ? 1 : 0; }
    ++out_iter;
  }
  return carry and AddWord(std::span<uintptr_t>(out_iter, out.end()), carry);
}

// Subtracts `words` from `out` and returns a pointer just passed the last
// non-zero word in `out`. Assumse that `words`
uintptr_t *SubWords(std::span<uintptr_t const> words,
                    std::span<uintptr_t> out) {
  NTH_REQUIRE((v.debug), words.size() <= out.size());
  auto out_iter     = out.begin();
  uintptr_t borrow  = 0;
  uintptr_t *result = &*out_iter;
  for (uintptr_t word : words) {
    if (*out_iter > word + borrow) {
      *out_iter -= word + borrow;
      borrow = 0;
      result = &*++out_iter;
    } else if (*out_iter == word + borrow) {
      *out_iter -= word + borrow;
      borrow = 0;
      ++out_iter;
    } else {
      *out_iter -= word + borrow;
      borrow = 1;
      result = &*++out_iter;
    }
  }

  if (borrow) {
    return SubWord(std::span<uintptr_t>(out_iter, out.end()), borrow);
  } else if (words.size() == out.size()) {
    return result;
  } else {
    return out.data() + out.size();
  }
}

uintptr_t *HeapAllocate(uintptr_t size) {
  return static_cast<uintptr_t *>(::operator new (
      size * sizeof(uintptr_t), std::align_val_t{alignof(uintptr_t)}));
}

void UncheckedDeallocate(uintptr_t *ptr) {
  ::operator delete (ptr, std::align_val_t{alignof(uintptr_t)});
}

void UncheckedResize(uintptr_t *&ptr, uintptr_t &size) {
  auto *p = HeapAllocate(size * 2);
  std::memcpy(p, ptr, sizeof(uintptr_t) * size);
  ::operator delete (ptr, std::align_val_t{alignof(uintptr_t)});
  ptr = p;
  size *= 2;
}

void MoveToHeap(uintptr_t (&data)[2]) {
  auto [low, hi] = data;
  auto *&ptr     = *reinterpret_cast<uintptr_t **>(&data[0]);
  ptr            = HeapAllocate(4);
  data[1]        = 4;
  ptr[0]         = low;
  ptr[1]         = hi;
}

}  // namespace

void integer::ResizeTo(uintptr_t new_size) {
  if (size_ > 2 and new_size <= 2) {
    auto *ptr = unchecked_data();
    data_[0]  = ptr[0];
    data_[1]  = ptr[1];
    UncheckedDeallocate(ptr);
  }
  size_ = new_size;
  if (new_size == 0) { sign_ = 0; }
}

integer::integer(long long n) : size_(n != 0), sign_(n < 0), data_{0, 0} {
  if (n >= 0) {
    data_[0] = n;
  } else if (n != std::numeric_limits<long long>::lowest()) {
    data_[0] = -n;
  } else {
    data_[1] = 1;
    size_    = 2;
  }
}

integer::integer(unsigned long long n)
    : size_(n != 0), sign_(n < 0), data_{static_cast<uintptr_t>(n), 0} {}

integer::integer(integer const &n)
    : size_(n.size_), sign_(n.sign_), data_{n.data_[0], n.data_[1]} {
  if (size_ > 2) {
    auto *ptr = HeapAllocate(size_);
    std::memcpy(ptr, unchecked_data(), sizeof(uintptr_t) * size_);
    data_[0] = reinterpret_cast<uintptr_t>(ptr);
    data_[1] = size_;
  }
}

integer::integer(integer &&n)
    : size_(n.size_),
      sign_(n.sign_),
      data_{std::exchange(n.data_[0], 0), std::exchange(n.data_[1], 0)} {
  n.size_ = 0;
}

integer &integer::operator=(integer const &n) {
  size_ = n.size_;
  sign_ = n.sign_;
  if (size_ <= 2) {
    data_[0] = n.data_[0];
    data_[1] = n.data_[1];
  } else {
    auto *ptr = HeapAllocate(size_);
    data_[0]  = reinterpret_cast<uintptr_t>(ptr);
    data_[1]  = size_;
    std::memcpy(ptr, unchecked_data(), sizeof(uintptr_t) * size_);
  }
  return *this;
}

integer &integer::operator=(integer &&n) {
  size_    = n.size_;
  n.size_  = 0;
  sign_    = n.sign_;
  data_[0] = std::exchange(n.data_[0], 0);
  data_[1] = std::exchange(n.data_[1], 0);
  return *this;
}

std::span<uintptr_t const> integer::words() const {
  switch (size_) {
    case 0:
    case 1:
    case 2: return std::span<uintptr_t const>(&data_[0], size_);
    default:
      return std::span<uintptr_t const>(
          reinterpret_cast<uintptr_t const *>(data_[0]), size_);
  }
}

std::span<uintptr_t> integer::words() {
  switch (size_) {
    case 0:
    case 1:
    case 2: return std::span<uintptr_t>(&data_[0], size_);
    default:
      return std::span<uintptr_t>(reinterpret_cast<uintptr_t *>(data_[0]),
                                  size_);
  }
}

integer integer::from_words(std::initializer_list<unsigned long long> words) {
  switch (words.size()) {
    case 0: return integer(0);
    case 1: return integer(*words.begin());
    case 2: {
      integer n;
      n.size_    = words.size();
      n.sign_    = 0;
      auto iter  = words.begin();
      n.data_[0] = *iter++;
      n.data_[1] = *iter;
      return n;
    }
    default: {
      integer n;
      n.size_    = words.size();
      n.sign_    = 0;
      auto *ptr  = HeapAllocate(n.size_);
      n.data_[0] = reinterpret_cast<uintptr_t>(ptr);
      n.data_[1] = n.size_;
      std::memcpy(ptr, words.begin(),
                  sizeof(unsigned long long) * words.size());
      return n;
    }
  }
}

integer &&integer::operator-() && {
  if (zero(*this)) { return std::move(*this); }
  sign_ = 1 - sign_;
  return std::move(*this);
}

integer operator+(integer const &lhs, integer const &rhs) {
  if (lhs.size_ < rhs.size_) { return rhs + lhs; }
  if (zero(rhs)) { return lhs; }
  // Can now assume `lhs` is at least as large as `rhs` in magnitude, and
  // neither are zero.

  if (sign(lhs) == sign(rhs)) {
    integer result = lhs;
    if (not AddWords(rhs.words(), result.words())) { return result; }

    // Handle the carry-bit.
    switch (result.size_) {
      default:
        if (result.size_ == result.unchecked_capacity()) {
          // We're at capacity and thus need to resize.
          UncheckedResize(result.unchecked_data(), result.data_[1]);
        }
        goto handle_same_sign_carry;
      case 2:
        // We're at capacity for the small-buffer and need to allocate space.
        MoveToHeap(result.data_);
        [[fallthrough]];
      case 1:
      handle_same_sign_carry:
        // We have space to expand, either because we just created space, or
        // because we fit in a single word and have an extra word's worth of
        // space in the small-buffer. Increment the size and write a `1` in
        // the next word for the carry.
        ++result.size_;
        result.words().back() = 1;
        return result;
      case 0: NTH_UNREACHABLE();
    }
  } else {
    integer result = lhs;
    auto *p        = SubWords(rhs.words(), result.words());
    if (p != result.words().data() + result.words().size()) {
      result.ResizeTo(p - result.words().data());
    } else if (p == result.words().data() + result.words().size()) {
      if (result.words().back() > lhs.words().back()) {
        // Subtraction at the last word caused a borrow.
        result.sign_ = 0;
        p            = result.words().data();
        for (uintptr_t &word : result.words()) {
          word = ~word;
          if (word != 0) { p = &word + 1; }
        }

        result.ResizeTo(p - result.words().data());

        ++result;
        result.sign_ = rhs.sign_;
      }
    }

    return result;
  }
}

void integer::ReduceMagnitudeByOne() {
  auto *p       = SubWord(words(), 1);
  auto *end_ptr = words().data() + words().size();
  if (p == end_ptr) { return; }
  switch (size_) {
    case 0: NTH_UNREACHABLE();
    case 1:
      sign_ = 0;
      size_ = 0;
      break;
    case 2: size_ = 1; break;
    case 3: {
      auto *ptr = unchecked_data();
      data_[0]  = ptr[0];
      data_[1]  = ptr[1];
      UncheckedDeallocate(ptr);
      size_ = 2;
    } break;
    default: --size_;
  }
}

void integer::IncreaseMagnitudeByOne() {
  if (not AddWord(words(), 1)) { return; }
  switch (size_) {
    case 0:
    case 1: data_[size_++] = 1; break;
    case 2: {
      MoveToHeap(data_);
      ++size_;
      words().back() = 1;
    } break;
    default:
      if (size_ == unchecked_capacity()) {
        UncheckedResize(unchecked_data(), data_[1]);
      }
      ++size_;
      words().back() = 1;
  }
}

integer &integer::operator++() & {
  negative(*this) ? ReduceMagnitudeByOne() : IncreaseMagnitudeByOne();
  return *this;
}

integer integer::operator++(int) {
  auto copy = *this;
  ++*this;
  return copy;
}

enum sign sign(integer const &n) {
  if (n.sign_) { return sign::negative; }
  return n.size_ ? sign::positive : sign::zero;
}

bool positive(integer const &n) { return n.size_ > 0 and not n.sign_; }
bool negative(integer const &n) { return n.sign_; }
bool zero(integer const &n) { return n.size_ == 0; }

bool operator==(integer const &lhs, integer const &rhs) {
  if (lhs.size_ != rhs.size_ or lhs.sign_ != rhs.sign_) { return false; }
  return std::memcmp(lhs.words().data(), rhs.words().data(),
                     sizeof(uintptr_t) * lhs.size_) == 0;
}

bool operator!=(integer const &lhs, integer const &rhs) {
  return not(lhs == rhs);
}

bool operator<(integer const &lhs, integer const &rhs) {
  return not(rhs <= lhs);
}

bool operator<=(integer const &lhs, integer const &rhs) {
  if (negative(lhs) != negative(rhs)) { return negative(lhs); }
  bool neg = negative(lhs);
  if (lhs.size_ < rhs.size_) { return not neg; }
  if (lhs.size_ > rhs.size_) { return neg; }
  auto lhs_iter = lhs.words().rbegin();
  auto rhs_iter = rhs.words().rbegin();
  for (; lhs_iter != lhs.words().rend(); ++lhs_iter, ++rhs_iter) {
    if (*lhs_iter < *rhs_iter) { return not neg; }
    if (*lhs_iter > *rhs_iter) { return neg; }
  }
  return true;
}

bool operator>(integer const &lhs, integer const &rhs) {
  return not(lhs <= rhs);
}

bool operator>=(integer const &lhs, integer const &rhs) { return rhs <= lhs; }

uintptr_t integer::unchecked_capacity() const {
  NTH_REQUIRE((v.debug), size_ > 2u);
  return data_[1];
}

uintptr_t const *integer::unchecked_data() const {
  NTH_REQUIRE((v.debug), size_ > 2u);
  return reinterpret_cast<uintptr_t const *>(data_[0]);
}

uintptr_t *&integer::unchecked_data() {
  NTH_REQUIRE((v.debug), size_ > 2u);
  return *reinterpret_cast<uintptr_t **>(&data_[0]);
}

std::string_view integer::PrintUsingBuffer(std::span<char> buffer) const {
  static constexpr std::string_view HexChars = "0123456789abcdef";

  auto end  = words().rend();
  char *ptr = buffer.data();
  auto iter = words().rbegin();
  size_t i  = 0;
  for (; i < 16; ++i) {
    if (((*iter >> (60 - 4 * i)) & 0xf) != 0) { break; }
  }
  for (; i < 16; ++i) { *ptr++ = HexChars[(*iter >> (60 - 4 * i)) & 0xf]; }
  ++iter;
  for (; iter != end; ++iter) {
    for (size_t i = 0; i < 16; ++i) {
      *ptr++ = HexChars[(*iter >> (60 - 4 * i)) & 0xf];
    }
  }
  return std::string_view(buffer.data(), ptr - buffer.data());
}

}  // namespace nth
