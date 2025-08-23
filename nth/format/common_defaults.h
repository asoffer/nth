#ifndef NTH_FORMAT_COMMON_DEFAULTS_H
#define NTH_FORMAT_COMMON_DEFAULTS_H

#include <concepts>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "nth/format/common_formatters.h"
#include "nth/meta/type.h"

namespace nth {

inline word_formatter<casing::lower> NthDefaultFormatter(
    nth::type_tag<decltype(nullptr)>) {
  return {};
}

inline word_formatter<casing::lower> NthDefaultFormatter(nth::type_tag<bool>) {
  return {};
}

inline ascii_formatter NthDefaultFormatter(nth::type_tag<char>) { return {}; }

inline trivial_formatter NthDefaultFormatter(nth::type_tag<std::monostate>) {
  return {};
}

inline byte_formatter NthDefaultFormatter(nth::type_tag<std::byte>) {
  return {};
}

template <std::convertible_to<std::string_view> T>
inline text_formatter NthDefaultFormatter(nth::type_tag<T>) {
  return {};
}

template <size_t N>
text_formatter NthDefaultFormatter(nth::type_tag<char[N]>) {
  return {};
}

template <typename T>
pointer_formatter NthDefaultFormatter(nth::type_tag<T*>) {
  return {};
}

template <std::integral I>
auto NthDefaultFormatter(nth::type_tag<I>) {
  return base_formatter(10);
}

inline auto NthDefaultFormatter(nth::type_tag<float>) {
  return float_formatter{};
}

inline auto NthDefaultFormatter(nth::type_tag<double>) {
  return float_formatter{};
}

}  // namespace nth

#endif  // NTH_FORMAT_COMMON_DEFAULTS_H
