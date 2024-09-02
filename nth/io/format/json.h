#ifndef NTH_IO_FORMAT_JSON_H
#define NTH_IO_FORMAT_JSON_H

#include <type_traits>

#include "nth/container/stack.h"
#include "nth/io/format/common.h"
#include "nth/io/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/types/structure.h"

namespace nth::io {
namespace internal_json {

template <typename>
struct json_structure {
  static constexpr structure value = structure::unknown;
};

template <typename T>
  requires std::is_arithmetic_v<T>
struct json_structure<T> {
  static constexpr structure value = structure::primitive;
};

template <nth::explicitly_convertible_to<std::string_view> T>
struct json_structure<T> {
  static constexpr structure value = structure::primitive;
};

template <typename T>
concept seq = requires(T t) {
  std::begin(t);
  std::end(t);
} and not nth::explicitly_convertible_to<T, std::string_view>;
template <seq T>
struct json_structure<T> {
  static constexpr structure value = structure::sequence;
};

template <seq T>
  requires(requires(T t) {
    typename T::key_type;
    typename T::mapped_type;
  })
struct json_structure<T> {
  static constexpr structure value = structure::associative;
};

}  // namespace internal_json

struct json_formatter : structural_formatter {
 private:
  template <structure S>
  using cv = nth::constant_value<S>;

 public:
  template <typename T>
  static constexpr structure structure_of =
      internal_json::json_structure<T>::value;

  void begin(cv<structure::object>, writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.close = "}", .width = 0});
  }

  void begin(cv<structure::associative>, writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.close = "}", .width = 0});
  }

  void begin(cv<structure::sequence>, writer auto &w) {
    io::write_text(w, "[");
    nesting_.push({.close = "]", .width = 0});
  }

  void begin(cv<structure::entry>, writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    if (nesting_.top().width++ != 0) { io::write_text(w, ","); }
    io::write_text(w, "\n");
    io::format(w, io::char_spacer(' ', 2 * nesting_.size()));
  }

  void begin(cv<structure::key>, writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    if (nesting_.top().width++ != 0) { io::write_text(w, ","); }
    io::write_text(w, "\n");
    io::format(w, io::char_spacer(' ', 2 * nesting_.size()));
  }

  void begin(cv<structure::value>, writer auto &) {}

  void end(cv<structure::associative>, writer auto &w) { this->end(w); }
  void end(cv<structure::object>, writer auto &w) { this->end(w); }
  void end(cv<structure::sequence>, writer auto &w) { this->end(w); }
  void end(cv<structure::entry>, writer auto &) { ++nesting_.top().width; }
  void end(cv<structure::key>, writer auto &w) { io::write_text(w, ": "); }
  void end(cv<structure::value>, writer auto &) { ++nesting_.top().width; }

  using structural_formatter::format;

  void format(writer auto &w, bool b) {
    io::write_text(w, b ? "true" : "false");
  }
  void format(writer auto &w, std::integral auto n) {
    base_formatter(10).format(w, n);
  }
  void format(writer auto &w, std::floating_point auto x) {
    float_formatter{}.format(w, x);
  }

  template <nth::explicitly_convertible_to<std::string_view> T>
  void format(writer auto &w, T const &s) {
    quote_formatter{}.format(w, static_cast<std::string_view>(s));
  }

private:
  void end(writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    auto [close, count] = nesting_.top();
    nesting_.pop();
    if (count != 0) {
      io::write_text(w, "\n");
      io::format(w, io::char_spacer(' ', 2 * nesting_.size()));
    }
    io::write_text(w, close);
  }

  struct nesting {
    char close[2];
    int width;
  };
  nth::stack<nesting> nesting_;
};

}  // namespace nth::io

#endif  // NTH_IO_FORMAT_JSON_H
