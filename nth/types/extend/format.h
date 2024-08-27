#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/format/format.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/reflect.h"

namespace nth {

template <typename T>
struct formatting : nth::extension<T> {
  friend nth::trivial_format_spec NthFormatSpec(nth::interpolation_string_view,
                                                nth::type_tag<T>) {
    return {};
  }

  friend void NthFormat(nth::io::forward_writer auto &w,
                        nth::format_spec<T> const &, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    nth::format(w, {}, "{");
    std::string_view separator = "";
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      (nth::interpolate<"{}.{} = {}">(w, std::exchange(separator, ", "),
                                      *name_ptr++, args),
       ...);
    });
    nth::format(w, {}, "}");
  }
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_FORMAT_H
