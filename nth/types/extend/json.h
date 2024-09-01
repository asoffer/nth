#ifndef NTH_TYPES_EXTEND_JSON_H
#define NTH_TYPES_EXTEND_JSON_H

#include <type_traits>

#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/types/extend/extend.h"
#include "nth/types/extend/format.h"

namespace nth::ext {
namespace internal_json {

enum class kind { number, string, array, associative, object };

template <typename>
struct category_impl {
  static constexpr kind value = kind::object;
};

template <typename T>
  requires std::is_arithmetic_v<T>
struct category_impl<T> {
  static constexpr kind value = kind::number;
};

template <nth::explicitly_convertible_to<std::string_view> T>
struct category_impl<T> {
  static constexpr kind value = kind::string;
};

template <typename T>
concept has_begin_and_end = requires(T t) {
  std::begin(t);
  std::end(t);
} and not nth::explicitly_convertible_to<T, std::string_view>;

template <typename T>
concept is_associative = has_begin_and_end<T> and requires(T t) {
  typename T::key_type;
  typename T::mapped_type;
};

template <has_begin_and_end T>
struct category_impl<T> {
  static constexpr kind value = kind::array;
};

template <is_associative T>
struct category_impl<T> {
  static constexpr kind value = kind::associative;
};

template <typename T>
inline constexpr kind category = category_impl<T>::value;

}  // namespace internal_json

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
    requires(internal_json::category<T> == internal_json::kind::array)
  void format(io::writer auto &w, T const &value) {
    begin_substructure(w, '[', ']');
    for (auto const &element : value) {
      begin_entry(w);
      nth::io::format(w, *this, element);
      end_entry(w);
    }
    end_substructure(w);
  }

  template <typename T>
  void format(io::writer auto &w, T const &value)
    requires(internal_json::category<T> == internal_json::kind::associative)
  {
    begin_substructure(w);
    for (auto const &[k, v] : value) {
      begin_entry(w);
      nth::io::format(w, *this, k);
      nth::io::write_text(w, ": ");
      nth::io::format(w, *this, v);
      end_entry(w);
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
    nth::reflect::on_fields<1>(value, [&](auto const &...args) {
      ((fmt.begin_entry(w),
        nth::io::format(w, fmt,
                        internal_format::entry{
                            .key   = *name_ptr++,
                            .value = args,
                        }),
        fmt.end_entry(w)),
       ...);
    });
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_JSON_H
