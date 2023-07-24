#include "nth/strings/edit_distance.h"

#include "nth/test/test.h"

namespace nth {

NTH_TEST("LevenshteinDistance/empty-strings") {
  NTH_EXPECT(LevenshteinDistance("", "") == size_t{0});
  NTH_EXPECT(LevenshteinDistance("abc", "") == size_t{3});
  NTH_EXPECT(LevenshteinDistance("", "xyz") == size_t{3});
}

NTH_TEST("LevenshteinDistance/transposition") {
  NTH_EXPECT(LevenshteinDistance("ab", "ba") == size_t{2});
  NTH_EXPECT(LevenshteinDistance("abcdef", "bacdef") == size_t{2});
}

NTH_TEST("LevenshteinDistance/different-lengths") {
  NTH_EXPECT(LevenshteinDistance("dog", "fido") == size_t{3});
  NTH_EXPECT(LevenshteinDistance("fido", "dog") == size_t{3});
  NTH_EXPECT(LevenshteinDistance("dog", "doggo") == size_t{2});
  NTH_EXPECT(LevenshteinDistance("doggo", "dog") == size_t{2});
}

}  // namespace nth
