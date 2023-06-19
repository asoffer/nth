#ifndef NTH_STRINGS_TEXT_H
#define NTH_STRINGS_TEXT_H

#include <string_view>

#include "nth/strings/text_encoding.h"

namespace nth {

// Finds the first position where a line-break may occur resulting in the
// largest possible line no longer than `line_width`. Line breaks may only occur
// at whitespace characters. The returned line and any surrounding whitespace
// are removed from `text`.
//
// More technically, returns the unique sub-string_view of `text` such that,
// there are only whitespace characters in `text` preceding it, its end either
// coinsides with the end of `text`, or is followed in text by a whitespace
// character, and it is the longest sub-string_view whose length is no more than
// `line_width` subject to these first two constraints. The input `text` then
// has a prefix removed containing the entirety of the returned sub-string_view
// and any whitespace characters immediately following it.
std::string_view GreedyLineBreak(std::string_view& text, size_t line_width,
                                 decltype(text_encoding::ascii));

}  // namespace nth

#endif  // NTH_STRINGS_TEXT_H
