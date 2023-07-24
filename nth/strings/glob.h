#ifndef NTH_STRINGS_GLOB_H
#define NTH_STRINGS_GLOB_H

#include <cstdlib>
#include <limits>
#include <string_view>

#include "nth/strings/internal/glob.h"

namespace nth {

// Returns true if the glob specified by `glob` matches the string `path`. Along
// with standard wildcards such as '*' and '?', `GlobMatches` also supports the
// globstar `**` wildcard.
inline constexpr bool GlobMatches(std::string_view glob,
                                  std::string_view path) {
  if (glob.empty()) { return path.empty(); }

  if (glob.size() > std::numeric_limits<uint16_t>::max() or
      path.size() > std::numeric_limits<uint16_t>::max()) {
    // Globs or paths with 2^16 or more characters are not supported.
    std::abort();
  }

  uint16_t chunk_starts[32];

  size_t index      = 0;
  size_t path_index = -1;
  do {
    if (index == 32) {
      // At most 32 sections (31 separating '/' characters) are supported.
      std::abort();
    }
    chunk_starts[index++] = path_index + 1;
    path_index            = path.find('/', path_index + 1);
  } while (path_index != std::string_view::npos);
  size_t const chunk_count  = index;

  uint32_t to_check = 1;

  while (not glob.empty()) {
    if (to_check == 0) { return false; }

    size_t glob_index          = glob.find('/');
    std::string_view glob_part = glob.substr(0, glob_index);
    if (glob_index == std::string_view::npos) {
      glob = "";
    } else {
      glob.remove_prefix(glob_index + 1);
    }

    bool glob_is_wildcard = internal_glob::IsWildcard(glob_part);

    if (glob_is_wildcard and glob_part == "**") {
      to_check = (~((to_check ^ (to_check - 1)) >> 1)) &
                 ((size_t{1} << (chunk_count + 1)) - 1);
    } else {
      size_t next_check_indices = 0;
      for (size_t check_indices = to_check; check_indices != 0;
           check_indices &= static_cast<uint32_t>(check_indices - 1)) {
        size_t index = internal_glob::LeastSetBitLocation(check_indices);
        if (index == chunk_count) { break; }

        if (glob_is_wildcard) {
          if (index != 31 and
              internal_glob::GlobChunkMatches(
                  glob_part, internal_glob::Chunk(path, chunk_starts, index,
                                                  chunk_count))) {
            next_check_indices |= (uint32_t{1} << (index + 1));
          }
        } else {
          if (index != 31 and
              glob_part == internal_glob::Chunk(path, chunk_starts, index,
                                                chunk_count)) {
            next_check_indices |= (uint32_t{1} << (index + 1));
          }
        }
      }
      to_check = next_check_indices;
    }
  }
  return to_check & (size_t{1} << chunk_count);
}

}  // namespace nth

#endif  // NTH_STRINGS_GLOB_H
