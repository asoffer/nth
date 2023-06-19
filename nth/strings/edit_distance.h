#ifndef NTH_STRINGS_DISTANCE_H_H
#define NTH_STRINGS_DISTANCE_H_H

#include <string_view>

namespace nth {

// Computes the Levenshtein distance between the strings viewed by `lhs` and
// `rhs`. The Levenshtein distance between two strings is the smallest number of
// character insertions, deletions, or replacements needed to transform one
// string into the other.
size_t LevenshteinDistance(std::string_view lhs, std::string_view rhs);

}  // namespace nth

#endif  // NTH_STRINGS_DISTANCE_H_H
