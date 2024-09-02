#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/format/cc.h"
#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/internal/format.h"
#include "nth/types/reflect.h"

namespace nth::ext {

template <typename T>
struct format : nth::extension<T> {
  friend io::cc_formatter NthDefaultFormatter(nth::type_tag<T>) { return {}; }

  friend void NthFormat(io::writer auto &w, auto &fmt, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    io::begin_format<structure::object>(w, fmt);
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      ((begin_format<structure::key>(w, fmt),
        nth::io::format(w, fmt, *name_ptr++),
        end_format<structure::key>(w, fmt),
        begin_format<structure::value>(w, fmt), nth::io::format(w, fmt, args),
        end_format<structure::value>(w, fmt)),
       ...);
    });
    io::end_format<structure::object>(w, fmt);
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_FORMAT_H
