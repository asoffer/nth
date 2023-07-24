#ifndef NTH_STRINGS_INTERNAL_GLOB_H
#define NTH_STRINGS_INTERNAL_GLOB_H

#include <iostream>
#include <cstdlib>
#include <string_view>

namespace nth::internal_glob {
inline constexpr bool IsWildcard(std::string_view pattern) {
  return pattern.find_first_of("?*[") != std::string_view::npos;
}

inline constexpr std::string_view Chunk(std::string_view path,
                                        uint16_t const *chunk_start_array,
                                        size_t index, size_t bound) {
  if (index + 1 == bound) { return path.substr(chunk_start_array[index]); }
  return path.substr(chunk_start_array[index],
                     chunk_start_array[index + 1] - chunk_start_array[index] - 1);
}

inline constexpr size_t LeastSetBitLocation(uint32_t n) {
  constexpr size_t DeBruijn[32] = {0,  1,  28, 2,  29, 14, 24, 3,  //
                                   30, 22, 20, 15, 25, 17, 4,  8,
                                   31, 27, 13, 23, 21, 19, 16, 7,
                                   26, 12, 18, 6,  11, 5,  10, 9};
  return DeBruijn[(static_cast<uint32_t>((n & -n) * uint32_t{0x077cb531})) >>
                  27];
}

inline constexpr bool GlobChunkMatches(std::string_view glob_part,
                                       std::string_view text_chunk) {
  while (true) {
    if (glob_part.empty()) { return text_chunk.empty(); }
    switch (glob_part[0]) {
      case '?': {
        if (text_chunk.empty()) { return false; }
        glob_part.remove_prefix(1);
        text_chunk.remove_prefix(1);

      } break;
      case '*': return true;          // TODO: This is incorrect.
      case '[': std::abort(); break;  // TODO: Not yet supported.
      default:
        if (text_chunk.empty() or text_chunk[0] != glob_part[0]) {
          return false;
        }
        glob_part.remove_prefix(1);
        text_chunk.remove_prefix(1);
    }
  }
  return true;
}

}  // namespace nth::internal_glob

#endif // NTH_STRINGS_INTERNAL_GLOB_H
