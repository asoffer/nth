#ifndef NTH_FORMAT_JSON_H
#define NTH_FORMAT_JSON_H

#include <type_traits>

#include "nth/container/stack.h"
#include "nth/format/common.h"
#include "nth/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/types/structure.h"

namespace nth {
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

struct json_formatter : structural_formatter<json_formatter> {
 private:
  template <structure S>
  using cv = nth::constant_value<S>;

 public:
  template <typename T>
  static constexpr structure structure_of =
      internal_json::json_structure<T>::value;

  void begin(cv<structure::object>, io::writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.close = "}", .width = 0});
  }

  void begin(cv<structure::associative>, io::writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.close = "}", .width = 0});
  }

  void begin(cv<structure::sequence>, io::writer auto &w) {
    io::write_text(w, "[");
    nesting_.push({.close = "]", .width = 0});
  }

  void begin(cv<structure::entry>, io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    if (nesting_.top().width++ != 0) { io::write_text(w, ","); }
    io::write_text(w, "\n");
    nth::format(w, char_spacer(' ', 2 * nesting_.size()));
  }

  void begin(cv<structure::key>, io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    if (nesting_.top().width++ != 0) { io::write_text(w, ","); }
    io::write_text(w, "\n");
    nth::format(w, char_spacer(' ', 2 * nesting_.size()));
  }

  void begin(cv<structure::value>, io::writer auto &) {}

  void end(cv<structure::associative>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::object>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::sequence>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::entry>, io::writer auto &) { ++nesting_.top().width; }
  void end(cv<structure::key>, io::writer auto &w) { io::write_text(w, ": "); }
  void end(cv<structure::value>, io::writer auto &) { ++nesting_.top().width; }

  using structural_formatter::format;

  void format(io::writer auto &w, bool b) {
    io::write_text(w, b ? "true" : "false");
  }
  void format(io::writer auto &w, std::integral auto n) {
    base_formatter(10).format(w, n);
  }
  void format(io::writer auto &w, std::floating_point auto x) {
    float_formatter{}.format(w, x);
  }

  template <nth::explicitly_convertible_to<std::string_view> T>
  void format(io::writer auto &w, T const &s) {
    quote_formatter{}.format(w, static_cast<std::string_view>(s));
  }

 private:
  void end(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    auto [close, count] = nesting_.top();
    nesting_.pop();
    if (count != 0) {
      io::write_text(w, "\n");
      nth::format(w, char_spacer(' ', 2 * nesting_.size()));
    }
    io::write_text(w, close);
  }

  struct nesting {
    char close[2];
    int width;
  };
  nth::stack<nesting> nesting_;
};

}  // namespace nth

#endif  // NTH_FORMAT_JSON_H
