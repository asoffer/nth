#ifndef NTH_TYPES_EXTEND_FORMAT_H
#define NTH_TYPES_EXTEND_FORMAT_H

#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/types/extend/extend.h"
#include "nth/types/reflect.h"

namespace nth {

template <typename T>
struct formatting : nth::extension<T> {
  friend void NthFormat(io::writer auto &w, auto &, T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    nth::io::write_text(w, "{");
    std::string_view separator = "";
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      (nth::interpolate<"{}.{} = {}">(w, std::exchange(separator, ", "),
                                      *name_ptr++, args),
       ...);
    });
    nth::io::write_text(w, "}");
  }
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_FORMAT_H
