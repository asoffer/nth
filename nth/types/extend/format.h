#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include <optional>

#include "nth/format/cc.h"
#include "nth/format/common.h"
#include "nth/format/format.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/internal/format.h"
#include "nth/types/reflect.h"

namespace nth::ext {

namespace internal_format {

void format_key_value(io::writer auto &w, auto &fmt, std::string_view key,
                      auto const &value) {
  nth::begin_format<structure::key>(w, fmt);
  nth::format(w, fmt, key);
  nth::end_format<structure::key>(w, fmt);
  nth::begin_format<structure::value>(w, fmt);
  nth::format(w, fmt, value);
  nth::end_format<structure::value>(w, fmt);
}

template <typename T>
void format_key_value(io::writer auto &w, auto &fmt, std::string_view key,
                      std::optional<T> const &value) {
  if (not value) { return; }
  nth::begin_format<structure::key>(w, fmt);
  nth::format(w, fmt, key);
  nth::end_format<structure::key>(w, fmt);
  nth::begin_format<structure::value>(w, fmt);
  nth::format(w, fmt, *value);
  nth::end_format<structure::value>(w, fmt);
}

}  // namespace internal_format

template <typename T>
struct format : nth::extension<T> {
  friend cc_formatter NthDefaultFormatter(nth::type_tag<T>) { return {}; }

  friend void NthFormat(io::writer auto &w, auto &fmt, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    nth::begin_format<structure::object>(w, fmt);
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      (internal_format::format_key_value(w, fmt, *name_ptr++, args), ...);
    });
    nth::end_format<structure::object>(w, fmt);
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_FORMAT_H
