#ifndef NTH_TYPE_EXTEND_INTERNAL_FORMAT_H
#define NTH_TYPE_EXTEND_INTERNAL_FORMAT_H

#include "nth/container/stack.h"
#include "nth/debug/unreachable.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"

namespace nth::ext::internal_format {

template <typename T>
struct entry {
  std::string_view key;
  T const &value;
};

struct basic_structural_formatter {
  explicit basic_structural_formatter(std::string_view separator = ",")
      : separator_(separator) {}
  void begin_substructure(io::writer auto &w, char open = '{',
                          char close = '}') {
    nesting_.push({.open = {open, '\0'}, .close = {close, '\0'}, .width = 0});
    nth::io::write_text(w, nesting_.top().open);
  }
  void end_substructure(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    auto [open, close, count] = nesting_.top();
    nesting_.pop();
    if (count != 0) {
      nth::io::write_text(w, "\n");
      nth::io::format(w, nth::io::char_spacer(' ', 2 * nesting_.size()));
    }
    nth::io::write_text(w, close);
  }

  void begin_entry(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    if (nesting_.top().width++ != 0) { nth::io::write_text(w, separator_); }
    nth::io::write_text(w, "\n");
    nth::io::format(w, nth::io::char_spacer(' ', 2 * nesting_.size()));
  }

  void end_entry(io::writer auto &) { ++nesting_.top().width; }

 private:
  std::string_view separator_;
  struct nesting {
    char open[2];
    char close[2];
    int width;
  };
  nth::stack<nesting> nesting_;
};

}  // namespace nth::ext::internal_format

#endif  // NTH_TYPE_EXTEND_INTERNAL_FORMAT_H
