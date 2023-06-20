#ifndef NTH_STRINGS_TEXT_H
#define NTH_STRINGS_TEXT_H

#include <string_view>

#include "nth/strings/text_encoding.h"

namespace nth {

// If `text` is empty, returns `text`. Otherwise, returns a non-empty
// sub-`std::string_view` of `text` subject to the following constraints, if
// possible:
//
//   * There are only whitespace characters in `text` preceding the returned
//     `std::string_view`.
//   * The end of the returned `std::string_view` occurs at the boundary between
//     a non-whitespace and a whitespace character.
//   * The longest possible `std::string_view` whose length is no more than
//     `line_width` and is subject to the first two constraints is returned.
//
// If no such `std::string_view` exists (because the very first contiguous
// sequence of non-whitespace characters is longer than `line_width`), then a
// view over the first contiguous sequence of non-whitespace characters is
// returned. In either case, the input `text` then has a prefix removed
// containing the entirety of the returned sub-`std::string_view` and any
// whitespace characters immediately preceding or following it.
std::string_view GreedyLineBreak(std::string_view& text, size_t line_width,
                                 decltype(text_encoding::ascii));

}  // namespace nth

#endif  // NTH_STRINGS_TEXT_H
