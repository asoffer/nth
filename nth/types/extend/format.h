#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/internal/format.h"
#include "nth/types/reflect.h"

namespace nth::ext {

struct designated_initializer_formatter
    : internal_format::basic_structural_formatter,
      nth::io::quote_formatter,
      nth::io::base_formatter {
  designated_initializer_formatter() : nth::io::base_formatter(10) {}

  void begin_entry(io::writer auto &w) {
    basic_structural_formatter::begin_entry(w);
    nth::io::write_text(w, ".");
  }

  template <typename T>
  void format(io::writer auto &w, internal_format::entry<T> const &e) {
    nth::io::format(w, e.key);
    nth::io::write_text(w, " = ");
    nth::io::format(w, e.value);
  }
};

template <typename T>
struct format : nth::extension<T> {
  friend designated_initializer_formatter NthDefaultFormatter(
      nth::type_tag<T>) {
    return {};
  }

  friend void NthFormat(io::writer auto &w, auto &fmt, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    io::with_substructure sub(w, fmt);
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      ((begin_entry(w, fmt),
        nth::io::format(w, fmt,
                        internal_format::entry{
                            .key   = *name_ptr++,
                            .value = args,
                        }),
        end_entry(w, fmt)),
       ...);
    });
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_FORMAT_H
