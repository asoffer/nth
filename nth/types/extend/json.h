#ifndef NTH_TYPES_EXTEND_JSON_H
#define NTH_TYPES_EXTEND_JSON_H

#include <iterator>

#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/format.h"

namespace nth::ext {

struct json_formatter : internal_format::basic_structural_formatter,
                        nth::io::quote_formatter,
                        nth::io::base_formatter {
  json_formatter() : nth::io::base_formatter(10) {}

  using nth::io::base_formatter::format;
  using nth::io::quote_formatter::format;

  template <typename T>
  void format(io::writer auto &w, internal_format::entry<T> const &e) {
    nth::io::format(w, *this, e.key);
    nth::io::write_text(w, ": ");
    nth::io::format(w, *this, e.value);
  }

  template <typename T>
  void format(io::writer auto &w, T const &value)
    requires(requires {
      std::begin(value);
      std::end(value);
    } and not nth::explicitly_convertible_to<T, std::string_view>)
  {
    begin_substructure(w, '[', ']');
    auto end_iter  = std::end(value);
    auto prev_iter = std::begin(value);
    if (prev_iter != end_iter) {
      for (auto curr_iter = prev_iter + 1; curr_iter != end_iter;
           prev_iter      = curr_iter, ++curr_iter) {
        begin_entry(w);
        nth::io::format(w, *this, *prev_iter);
        end_entry(w);
      }

      begin_entry(w);
      nth::io::format(w, *this, *prev_iter);
      end_entry(w, true);
    }
    end_substructure(w);
  }

 private:
  nth::stack<int> sizes_;
};

template <typename T>
struct format_json : nth::extension<T, format> {
  friend void NthFormat(io::writer auto &w, json_formatter &fmt,
                        T const &value) {
    std::string_view const *name_ptr =
        nth::reflect::field_names<1>(value).data();
    io::with_substructure sub(w, fmt);
    size_t i = 0;
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      size_t limit = sizeof...(args);
      ((fmt.begin_entry(w),
        nth::io::format(w, fmt,
                        internal_format::entry{
                            .key   = *name_ptr++,
                            .value = args,
                        }),
        fmt.end_entry(w, ++i == limit)),
       ...);
    });
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_JSON_H
