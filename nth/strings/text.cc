#include "nth/strings/text.h"

#include <cctype>

namespace nth {

std::string_view GreedyLineBreak(std::string_view& text, size_t line_width,
                                 decltype(text_encoding::ascii)) {
  char const* p = text.data();
  while (p != text.end() and std::isspace(*p)) { ++p; }
  if (p == text.end()) { return text = std::string_view(p, 0); }
  char const* first_word_start = p;

  char const* last_word_end   = p;
  char const* last_word_start = first_word_start;
  while (true) {
    while (p != text.end() and not std::isspace(*p)) { ++p; }
    char const* next_word_end = p;
    while (p != text.end() and std::isspace(*p)) { ++p; }
    char const* next_word_start = p;
    if (static_cast<size_t>(std::distance(first_word_start, next_word_end)) >
        line_width) {
      if (last_word_end == first_word_start) {
        text = std::string_view(next_word_start, text.end());
        return std::string_view(first_word_start, next_word_end);
      } else {
        text = std::string_view(last_word_start, text.end());
        return std::string_view(first_word_start, last_word_end);
      }
    }
    if (p == text.end()) {
      text = std::string_view(p, 0);
      return std::string_view(first_word_start, next_word_end);
    }
    last_word_end   = next_word_end;
    last_word_start = next_word_start;
  }
}

}  // namespace nth
