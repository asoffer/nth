#ifndef NTH_IO_FORMAT_COMMON_FORMATTERS_H
#define NTH_IO_FORMAT_COMMON_FORMATTERS_H

#include <charconv>

#include "nth/io/writer/writer.h"

namespace nth {

// A formatter which has no opinions about how objects of any type should be
// formatted.
struct trivial_formatter {};

// A formatter capable of formatting booleans and and `nullptr` as words. The
// template `casing` enumerator parameter indicates whether the words "true",
// "false", or "null" should be displaed lower-case, UPPER-CASE, or Title-Case.
enum class casing { upper, lower, title };
template <casing C>
struct word_formatter {
  void format(io::writer auto& w, bool b) const {
    if constexpr (C == casing::upper) {
      io::write_text(w, b ? "TRUE" : "FALSE");
    } else if constexpr (C == casing::title) {
      io::write_text(w, b ? "True" : "False");
    } else if constexpr (C == casing::lower) {
      io::write_text(w, b ? "true" : "false");
    }
  }

  void format(io::writer auto& w, decltype(nullptr)) const {
    if constexpr (C == casing::upper) {
      io::write_text(w, "NULL");
    } else if constexpr (C == casing::title) {
      io::write_text(w, "Null");
    } else if constexpr (C == casing::lower) {
      io::write_text(w, "null");
    }
  }
};

// A formatter capable of formatting `char` as the ascii character it
// represents.
struct ascii_formatter {
  void format(io::writer auto& w, char c) const {
    io::write_text(w, std::string_view(&c, 1));
  }
};

// A formatter capable of formatting integral types as numbers in a base
// between 2 and 36 inclusive.
struct base_formatter {
  explicit constexpr base_formatter(size_t base) : base_(base) {}

  void format(io::writer auto& w, bool b) const {
    io::write_text(w, b ? "1" : "0");
  }

  void format(io::writer auto& w, std::integral auto n) const {
    constexpr size_t buffer_size = 1 + static_cast<int>(sizeof(n) * 8);
    char buffer[buffer_size]     = {};
    auto result = std::to_chars(&buffer[0], &buffer[buffer_size], n, base_);
    io::write_text(w, std::string_view(buffer, result.ptr));
  }

 private:
  size_t base_;
};

struct float_formatter {
  void format(io::writer auto& w, std::floating_point auto x) const {
    // TODO: Determine the right buffer size.
    char buffer[1024] = {};
    auto result       = std::to_chars(&buffer[0], &buffer[1024], x);
    io::write_text(w, std::string_view(buffer, result.ptr));
  }
};

// A formatter capable of formatting text as specified, as if by a direct call
// to `io::write_text`.
struct text_formatter {
  void format(io::writer auto& w, std::string_view s) const {
    io::write_text(w, s);
  }
};

// A formatter capable of formatting text as an escaped quotation.
struct quote_formatter {
  void format(io::writer auto& w, std::string_view s) const {
    io::write_text(w, R"(")");
    size_t i = 0;
    for (char c : s) {
      switch (c) {
        case '\n':
          io::write_text(w, s.substr(0, i));
          s.remove_prefix(i + 1);
          i = 0;
          io::write_text(w, R"(\n)");
          break;
        case '"':
          io::write_text(w, s.substr(0, i));
          s.remove_prefix(i + 1);
          i = 0;
          io::write_text(w, R"(\")");
          break;
        default:
          if (std::isprint(c)) {
            ++i;
            continue;
          } else {
            io::write_text(w, s.substr(0, i));
            s.remove_prefix(i);
            i = 0;
          }
      }
    }
    io::write_text(w, s);
    io::write_text(w, R"(")");
  }
};

struct byte_formatter {
  void format(io::writer auto& w, std::byte b) const {
    constexpr char hex[] = "0123456789abcdef";
    char buffer[2];
    uint8_t n = static_cast<uint8_t>(b);
    buffer[0] = hex[n >> 4];
    buffer[1] = hex[n & 0x0f];
    io::write_text(w, buffer);
  }
};

struct pointer_formatter {
  void format(io::writer auto& w, void const* ptr) const {
    constexpr char hex[]                   = "0123456789abcdef";
    char buffer[sizeof(uintptr_t) * 2 + 2] = {'0', 'x'};

    uintptr_t n = reinterpret_cast<uintptr_t>(ptr);
    for (char* ptr = &buffer[sizeof(uintptr_t) * 2]; ptr != &buffer[2];
         ptr -= 2) {
      ptr[0] = hex[(n & uintptr_t{0xff}) >> 4];
      ptr[1] = hex[n & 0x0f];
      n >>= 8;
    }
    io::write_text(w, buffer);
  }
};

}  // namespace nth

#endif  // NTH_FORMAT_COMMON_FORMATTERS_H
