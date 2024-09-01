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
  void begin_substructure(io::writer auto &w, char open = '{',
                          char close = '}') {
    nesting_.push({.open = {open, '\0'}, .close = {close, '\0'}, .width = 0});
    nth::io::write_text(w, nesting_.top().open);
  }
  void end_substructure(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    auto [open, close, count] = nesting_.top();
    nesting_.pop();
    if (count != 0) { nth::io::write_text(w, "\n"); }

    nth::io::format(w, nth::io::char_spacer(' ', 2 * nesting_.size()));
    nth::io::write_text(w, close);
  }

  void begin_entry(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    ++nesting_.top().width;
    nth::io::write_text(w, "\n");
    nth::io::format(w, nth::io::char_spacer(' ', 2 * nesting_.size()));
  }

  void end_entry(io::writer auto &w, bool last = false) {
    ++nesting_.top().width;
    if (not last) { nth::io::write_text(w, ","); }
  }

 private:
  struct nesting {
    char open[2];
    char close[2];
    int width;
  };
  nth::stack<nesting> nesting_;
};

}  // namespace nth::ext::internal_format

#endif  // NTH_TYPE_EXTEND_INTERNAL_FORMAT_H
