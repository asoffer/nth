#ifndef NTH_FORMAT_FORMAT_H
#define NTH_FORMAT_FORMAT_H

#include "nth/format/common_defaults.h"
#include "nth/format/common_formatters.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/constant.h"
#include "nth/meta/type.h"
#include "nth/types/structure.h"

// This header file defines a mechanism by which one can register format
// specifications for a type. Format specifications can be used when printing or
// serializing values of a type to configure how the value is written.

namespace nth {

// Returns the default formatter associated with type `T`. If a function named
// `NthDefaultFormatter` exists, findable via argument-dependent lookup that
// accepts an `nth::type_tag<T>`, the result of executing
// `NthDefaultFormatter(nth::type<T>)` will be used as the default. Otherwise
// `nth::trivial_formatter will be used.
template <typename T>
constexpr auto default_formatter() {
  if constexpr (requires { NthDefaultFormatter(nth::type<T>); }) {
    return NthDefaultFormatter(nth::type<T>);
  } else {
    return trivial_formatter{};
  }
}

// Formats `value` with the formatter `fmt`, writing the result to `w`. If
// `fmt.format(w, value)` is a valid expression, this expression is
// evaluated and returned. Otherwise (if `fmt` does not say anything about
// how the value should be printed, then `NthFormat(w, fmt, value)` is
// evaluated and returned. The intended use is to provide both the type `T`
// of the value being formatted and the type of the formatter `F` to express
// opinions about how the value should be formatted. The design gives
// precedence to the formatter.
template <int&..., typename F, typename T>
constexpr auto format(io::writer auto& w, F&& fmt, T const& value) {
  if constexpr (requires { fmt.format(w, value); }) {
    return fmt.format(w, value);
  } else {
    return NthFormat(w, fmt, value);
  }
}

// Invokes the three-argument `format`, by constructing a local
// `default_formatter<T>()` and passing that as the formatter `fmt`.
template <int&..., typename T>
constexpr auto format(io::writer auto& w, T const& value) {
  nth::format(w, nth::default_formatter<T>(), value);
}

namespace internal_format {
template <typename F, nth::structure S>
concept semistructural_formatter =
    (requires(F f, io::minimal_writer w) { f.begin(nth::constant<S>, w); }) or
    (requires(F f, io::minimal_writer w) { f.end(nth::constant<S>, w); });

template <typename F, nth::structure S>
concept structural_formatter = requires(F f, io::minimal_writer w) {
  f.begin(nth::constant<S>, w);
  f.end(nth::constant<S>, w);
};

}  // namespace internal_format

template <nth::structure S, int&..., typename F>
decltype(auto) begin_format(io::writer auto& w, F& f) {
  if constexpr (internal_format::structural_formatter<F, S>) {
    return f.begin(nth::constant<S>, w);
  } else {
    constexpr bool IsStructural =
        not internal_format::semistructural_formatter<F, S>;
    static_assert(IsStructural, R"(
    The fromatter is not structural even though it has one of either
    `begin_format` or `end_format`. Both must be implemented. Did you forget or
    misspell one?
  )");
  }
}

template <nth::structure S, int&..., typename F>
decltype(auto) end_format(io::writer auto& w, F& f) {
  if constexpr (internal_format::structural_formatter<F, S>) {
    return f.end(nth::constant<S>, w);
  } else {
    constexpr bool IsStructural =
        not internal_format::semistructural_formatter<F, S>;
    static_assert(IsStructural, R"(
    The fromatter is not structural even though it has one of either
    `begin_format` or `end_format`. Both must be implemented. Did you forget or
    misspell one?
  )");
  }
}

void format_key_value(io::writer auto& w, auto& fmt, auto const& key,
                      auto const& value) {
  nth::begin_format<nth::structure::key>(w, fmt);
  nth::format(w, fmt, key);
  nth::end_format<nth::structure::key>(w, fmt);
  nth::begin_format<nth::structure::value>(w, fmt);
  nth::format(w, fmt, value);
  nth::end_format<nth::structure::value>(w, fmt);
}

// `structural_formatter` is a CRTP base-class that can be used to simplify
// formatting of objects which have a well-understood structure that is visible
// in the object being formatted. Users may implement begin/end pairs of member
// functions to be invoked immediately before and after an object of that
// structural category is formatted.
template <typename F>
struct structural_formatter {
  template <typename T>
  void format(io::writer auto& w, T const& value)
    requires(F::template structure_of<T> == structure::sequence)
  {
    begin_format<structure::sequence>(w, self());
    for (auto const& element : value) {
      begin_format<structure::entry>(w, self());
      nth::format(w, self(), element);
      end_format<structure::entry>(w, self());
    }
    end_format<structure::sequence>(w, self());
  }

  template <typename T>
  void format(io::writer auto& w, T const& value)
    requires(F::template structure_of<T> == structure::associative)
  {
    begin_format<structure::associative>(w, self());
    for (auto const& [k, v] : value) {
      begin_format<structure::key>(w, self());
      nth::format(w, self(), k);
      end_format<structure::key>(w, self());

      begin_format<structure::value>(w, self());
      nth::format(w, self(), v);
      end_format<structure::value>(w, self());
    }
    end_format<structure::associative>(w, self());
  }

 private:
  F& self() { return static_cast<F&>(*this); }
};

template <typename F, typename T>
void NthFormat(nth::io::writer auto& w, F&, T const& t) {
  char buffer[sizeof(T) * 3 + 1];
  buffer[0]                      = '[';
  constexpr std::string_view Hex = "0123456789abcdef";
  char* p                        = buffer;
  for (std::byte b : nth::bytes(t)) {
    *++p = Hex[static_cast<uint8_t>(b) >> 4];
    *++p = Hex[static_cast<uint8_t>(b) & 0x0f];
    *++p = ' ';
  }
  *p = ']';

  nth::io::write_text(w, std::string_view(nth::type<T>.name()));
  nth::io::write_text(w, std::string_view(buffer, 3 * sizeof(T) + 1));
}

}  // namespace nth

#endif  // NTH_FORMAT_FORMAT_H
