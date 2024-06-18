#ifndef NTH_STRINGS_INTERPOLATE_INTERNAL_VALIDATE_H
#define NTH_STRINGS_INTERPOLATE_INTERNAL_VALIDATE_H

#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>

namespace nth::internal_format {

[[maybe_unused]] void validation_failed();

inline constexpr bool following_code_unit(char c) {
  return (c & 0b1100'0000) == 0b1000'0000;
}

inline constexpr std::optional<uint32_t> extract_next_utf8_codepoint(
    std::string_view& text) {
  uint32_t value;
  if constexpr (std::is_signed_v<char>) {
    if (text[0] >= 0) {
      value = text[0];
    } else {
      value = static_cast<uint32_t>(static_cast<int32_t>(text[0]) + 128);
    }
  } else {
    value = text[0];
  }
  auto bytes = std::countl_one(value);
  if (text.size() < static_cast<size_t>(bytes)) { return std::nullopt; }
  switch (bytes) {
    case 0: text.remove_prefix(1); break;
    case 2:
      if (not following_code_unit(text[1])) { return std::nullopt; }
      value = ((value & 0b0001'1111) << 6) | static_cast<uint32_t>(text[1]);
      break;
    case 3:
      if (not following_code_unit(text[1])) { return std::nullopt; }
      if (not following_code_unit(text[2])) { return std::nullopt; }
      value = ((value & 0b0000'1111) << 12) |
              ((0b0011'1111 & static_cast<uint32_t>(text[1])) << 6) |
              (0b0011'1111 & static_cast<uint32_t>(text[2]));
      break;
    case 4:
      if (not following_code_unit(text[1])) { return std::nullopt; }
      if (not following_code_unit(text[2])) { return std::nullopt; }
      if (not following_code_unit(text[3])) { return std::nullopt; }
      value = ((value & 0b0000'0111) << 18) |
              ((0b0011'1111 & static_cast<uint32_t>(text[1])) << 12) |
              ((0b0011'1111 & static_cast<uint32_t>(text[2])) << 6) |
              (0b0011'1111 & static_cast<uint32_t>(text[3]));
      break;
    default: return std::nullopt;
  }
  text.remove_prefix(bytes);
  return value;
}

inline constexpr void validate_interpolation_string(
    std::string_view interpolation_string) {
  int brace_depth = 0;
  while (not interpolation_string.empty()) {
    if (auto codepoint = extract_next_utf8_codepoint(interpolation_string)) {
      switch (*codepoint) {
        case '{': ++brace_depth; break;
        case '}': {
          if (brace_depth == 0) {
            validation_failed(/* `}` without matching `{`. */);
          }
          --brace_depth;
        } break;
      }
    } else {
      validation_failed(/* invalid valid utf8 codepoint. */);
    }
  }
  if (brace_depth != 0) { validation_failed(/* mismatched braces. */); }
}

}  // namespace nth::internal_format

#endif  // NTH_STRINGS_INTERPOLATE_INTERNAL_VALIDATE_H
