#ifndef NTH_IO_STRING_PRINTER_H
#define NTH_IO_STRING_PRINTER_H

#include <string>
#include <string_view>

namespace nth {

// `string_printer` may be constructed with a `std::string&`. All writes to the
// printer will append to the referenced string.
struct string_printer {
  explicit constexpr string_printer(std::string& s) : s_(s) {}
  void write(size_t n, char c) { s_.append(n, c); }
  void write(std::string_view s) { s_.append(s); }

 private:
  std::string& s_;
};

// `bounded_string_printer` may be constructed with a `std::string&`, and a
// `bound`. All writes to the printer will append to the referenced string,
// until the string reaches the size `bound` at which point future writes (or
// the remainder of a current write, if the bound is reached during a call to
// `write`) are ignored.
//
// Note: The bound is on the number of characters written by the printer, not
// the total size of the referenced string. If another printer writes to the
// same string, or the string is not empty upon construction, the total size of
// the string may exceed the given bound.
struct bounded_string_printer {
  explicit constexpr bounded_string_printer(std::string& s, size_t bound)
      : s_(s), left_(bound) {}

  void write(size_t n, char c) {
    size_t to_write = std::min(n, left_);
    s_.append(to_write, c);
    left_ -= to_write;
  }

  void write(std::string_view s) {
    size_t to_write = std::min(s.size(), left_);
    s_.append(s.substr(0, to_write));
    left_ -= to_write;
  }

 private:
  std::string& s_;
  size_t left_;
};

}  // namespace nth

#endif  // NTH_IO_STRING_PRINTER_H
