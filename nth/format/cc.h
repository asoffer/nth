#ifndef NTH_FORMAT_CC_H
#define NTH_FORMAT_CC_H

#include <type_traits>

#include "nth/container/stack.h"
#include "nth/format/common.h"
#include "nth/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/types/structure.h"

namespace nth {
namespace internal_cc {

template <typename>
struct cc_structure {
  static constexpr structure value = structure::unknown;
};

template <typename T>
  requires std::is_arithmetic_v<T>
struct cc_structure<T> {
  static constexpr structure value = structure::primitive;
};

template <nth::explicitly_convertible_to<std::string_view> T>
struct cc_structure<T> {
  static constexpr structure value = structure::primitive;
};

template <typename T>
concept seq = requires(T t) {
  std::begin(t);
  std::end(t);
} and not nth::explicitly_convertible_to<T, std::string_view>;
template <seq T>
struct cc_structure<T> {
  static constexpr structure value = structure::sequence;
};

template <seq T>
  requires(requires(T t) {
    typename T::key_type;
    typename T::mapped_type;
  })
struct cc_structure<T> {
  static constexpr structure value = structure::associative;
};

}  // namespace internal_cc

struct cc_formatter : structural_formatter<cc_formatter> {
 private:
  template <structure S>
  using cv = nth::constant_value<S>;

 public:
  template <typename T>
  static constexpr structure structure_of = internal_cc::cc_structure<T>::value;

  void begin(cv<structure::associative>, io::writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.kind = structure::associative, .width = 0});
  }

  void begin(cv<structure::object>, io::writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.kind = structure::object, .width = 0});
  }

  void begin(cv<structure::sequence>, io::writer auto &w) {
    io::write_text(w, "{");
    nesting_.push({.kind = structure::sequence, .width = 0});
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

    switch (nesting_.top().kind) {
      case structure::object: io::write_text(w, "."); break;
      default: io::write_text(w, "{"); break;
    }
  }

  void begin(cv<structure::value>, io::writer auto &) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    nesting_.top().in_value = true;
  }

  void end(cv<structure::associative>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::object>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::sequence>, io::writer auto &w) { this->end(w); }
  void end(cv<structure::entry>, io::writer auto &) { ++nesting_.top().width; }
  void end(cv<structure::key>, io::writer auto &w) {
    switch (nesting_.top().kind) {
      case structure::object: io::write_text(w, " = "); break;
      case structure::associative: io::write_text(w, ", "); break;
      default:
        NTH_UNREACHABLE("kind = {}") <<=
            {static_cast<int>(nesting_.top().kind)};
    }
  }
  void end(cv<structure::value>, io::writer auto &w) {
    ++nesting_.top().width;
    if (nesting_.empty() or nesting_.top().kind != structure::object) {
      io::write_text(w, "}");
    }
    nesting_.top().in_value = false;
  }

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
    if (not nesting_.empty() and nesting_.top().kind == structure::object and
        not nesting_.top().in_value) {
      io::write_text(w, static_cast<std::string_view>(s));
    } else {
      quote_formatter{}.format(w, static_cast<std::string_view>(s));
    }
  }

 private:
  void end(io::writer auto &w) {
    if (nesting_.empty()) { NTH_UNREACHABLE(); }
    int count = nesting_.top().width;
    nesting_.pop();
    if (count != 0) {
      io::write_text(w, "\n");
      nth::format(w, char_spacer(' ', 2 * nesting_.size()));
    }
    io::write_text(w, "}");
  }

  struct nesting {
    structure kind;
    int width;
    bool in_value = false;
  };
  stack<nesting> nesting_;
};

}  // namespace nth

#endif  // NTH_FORMAT_CC_H
