#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/container/stack.h"
#include "nth/debug/unreachable.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/reflect.h"

namespace nth {

struct designated_initializer_formatter {
  void begin_substructure(io::writer auto &w) {
    nth::io::write_text(w, "{");
    sizes_.push(0);
  }
  void end_substructure(io::writer auto &w) {
    if (sizes_.empty()) { NTH_UNREACHABLE(); }
    size_t count = sizes_.top();
    sizes_.pop();
    if (count != 0) { nth::io::write_text(w, "\n"); }
    std::string_view indent = "  ";
    for (size_t i = 0; i < sizes_.size(); ++i) { io::write_text(w, indent); }
    nth::io::write_text(w, "}");
  }

  void begin_entry(io::writer auto &w) {
    if (sizes_.empty()) { NTH_UNREACHABLE(); }
    ++sizes_.top();
    nth::io::write_text(w, "\n");
    std::string_view indent = "  ";
    for (size_t i = 0; i < sizes_.size(); ++i) { io::write_text(w, indent); }
  }
  void end_entry(io::writer auto &w) {
    ++sizes_.top();
    nth::io::write_text(w, ",");
  }

 private:
  nth::stack<int> sizes_;
};

template <typename T>
struct formatting : nth::extension<T> {
  friend designated_initializer_formatter NthDefaultFormatter(
      nth::type_tag<T>) {
    return {};
  }

  friend void NthFormat(io::writer auto &w, auto &fmt, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    io::with_substructure sub(w, fmt);
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      ((begin_entry(w, fmt), nth::interpolate<".{} = {}">(w, *name_ptr++, args),
        end_entry(w, fmt)),
       ...);
    });
  }
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_FORMAT_H
