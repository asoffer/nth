#ifndef NTH_STRINGS_NULL_TERMINATED_STRING_VIEW_H
#define NTH_STRINGS_NULL_TERMINATED_STRING_VIEW_H

#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>

#include "nth/base/attributes.h"

namespace nth {

struct null_terminated_string_view {
 private:
  struct from_pointer_tag {};
  struct from_array_tag {};

 public:
  static constexpr from_pointer_tag from_pointer;
  static constexpr from_array_tag from_array;

  using value_type      = char;
  using pointer         = char *;
  using const_pointer   = char const *;
  using reference       = char &;
  using const_reference = char const &;

  using const_iterator  = const_pointer;
  using iterator        = pointer;
  using size_type       = size_t;
  using difference_type = ptrdiff_t;

  // Constructs a `null_terminated_string_view` pointing to an empty-string.
  constexpr null_terminated_string_view() = default;

  // Copy consturctor
  constexpr null_terminated_string_view(null_terminated_string_view const &) = default;

  // Constructs a `null_terminated_string_view` pointing to the given `data`
  // under the assumption that `data` is a pointer to a null-terminated char
  // array. The length of the array is determined as if by `std::strlen`.
  explicit constexpr null_terminated_string_view(from_pointer_tag,
                                                 char const *data)
      : data_(data), size_([&]() -> size_t {
          // clang-format off
          if consteval {
              auto *p = data;
              while (*p != '\0') { ++p; }
              return p - data;
          } else {
            return std::strlen(data);
          }
          // clang-format on
        }()) {}

  // Constructs a `null_terminated_string_view` pointing to the given `data`.
  // The last character (at index `N - 1`) must be null.
  template <size_t N>
  constexpr null_terminated_string_view(from_array_tag, char const (&data)[N]);

  // Constructs a `null_terminated_string_view` viewing the contents of `s`.
  constexpr null_terminated_string_view(
      std::string const &s NTH_ATTRIBUTE(lifetimebound))
      : data_(s.c_str()), size_(s.size()) {}

  [[nodiscard]] constexpr const_iterator begin() const { return data_; }
  [[nodiscard]] constexpr const_iterator end() const { return data_ + size_; }
  [[nodiscard]] constexpr const_iterator cbegin() const { return data_; }
  [[nodiscard]] constexpr const_iterator cend() const { return data_ + size_; }

  [[nodiscard]] constexpr char const *data() const { return data_; }
  [[nodiscard]] constexpr size_t size() const { return size_; }
  [[nodiscard]] constexpr bool empty() const { return size_ == 0; }

  [[nodiscard]] friend constexpr bool operator==(
      null_terminated_string_view lhs, null_terminated_string_view rhs) {
    return lhs.size() == rhs.size() and
           std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
  }

  constexpr operator std::string_view () const {
    return std::string_view(data_, size_);
  }


  void remove_prefix(size_t length) {
    // NTH_REQUIRE((v.harden), length <= size_);
    data_ += length;
    size_ -= length;
  }

  [[nodiscard]] char operator[](size_t n) const {
    // NTH_REQUIRE((v.harden), n <= size_);
    return data_[n];
  }

 private:
  char const *data_ = "";
  size_t size_      = 0;
};

template <size_t N>
constexpr null_terminated_string_view::null_terminated_string_view(
    from_array_tag, char const (&data)[N])
    : data_(data), size_(N - 1) {
  // NTH_REQUIRE((v.harden), data[N - 1] == 0);
}

}  // namespace nth

#endif  // NTH_STRINGS_NULL_TERMINATED_STRING_VIEW_H
