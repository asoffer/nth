#include "nth/strings/edit_distance.h"

#include <memory>
#include <string_view>

namespace nth {
namespace internal_edit_distance {

size_t LevenshteinDistanceImpl(std::string_view small, std::string_view big) {
  size_t size = small.size() + 1;
  std::unique_ptr<size_t[]> buffer(new size_t[size]);
  for (size_t i = 0; i < size; ++i) { buffer[i] = i; }

  for (size_t i = 1; i <= big.size(); ++i) {
    size_t previous = std::exchange(buffer[0], i);

    for (size_t j = 1; j < size; ++j) {
      previous = std::exchange(
          buffer[j],
          std::min({buffer[j - 1] + 1, previous + (small[j - 1] != big[i - 1]),
                    buffer[j] + 1}));
    }
  }
  return buffer[size - 1];
}

}  // namespace internal_edit_distance

size_t LevenshteinDistance(std::string_view lhs, std::string_view rhs) {
  if (lhs.size() > rhs.size()) { std::swap(lhs, rhs); }
  if (lhs.empty()) { return rhs.size(); }
  return internal_edit_distance::LevenshteinDistanceImpl(lhs, rhs);
}

}  // namespace nth
