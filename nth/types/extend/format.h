#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/format/cc.h"
#include "nth/format/common.h"
#include "nth/format/format.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/internal/format.h"
#include "nth/types/reflect.h"

namespace nth::ext {

template <typename T>
struct format : nth::extension<T> {
  friend cc_formatter NthDefaultFormatter(nth::type_tag<T>) { return {}; }

  friend void NthFormat(io::writer auto &w, auto &fmt, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    nth::begin_format<structure::object>(w, fmt);
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      ((nth::begin_format<structure::key>(w, fmt),
        nth::format(w, fmt, *name_ptr++),
        nth::end_format<structure::key>(w, fmt),
        nth::begin_format<structure::value>(w, fmt), nth::format(w, fmt, args),
        nth::end_format<structure::value>(w, fmt)),
       ...);
    });
    nth::end_format<structure::object>(w, fmt);
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_FORMAT_H
