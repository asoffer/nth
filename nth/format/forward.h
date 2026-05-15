#ifndef NTH_FORMAT_FORWARD_H
#define NTH_FORMAT_FORWARD_H

#include <string>

#include "nth/format/concept.h"
#include "nth/io/writer/writer.h"

namespace nth {

struct default_formatter_t;

template <int&..., nth::io::writer W, typename F, nth::formattable_with<W, F> T>
constexpr auto format(W& w, F&& fmt, T const& value);

template <int&..., nth::io::writer W,
          nth::formattable_with<W, default_formatter_t> T>
constexpr auto format(io::writer auto& w, T const& value);

template <int&..., typename F, typename T>
std::string format_to_string(F&& fmt, T const& value);

template <int&..., typename T>
std::string format_to_string(T const& value);

}  // namespace nth

#endif  // NTH_FORMAT_FORWARD_H
