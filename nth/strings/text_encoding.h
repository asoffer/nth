#ifndef NTH_STRINGS_TEXT_ENCODING_H
#define NTH_STRINGS_TEXT_ENCODING_H

namespace nth::text_encoding {
namespace internal_text {
struct AsciiEncoding {};
}  // namespace internal_text

inline constexpr internal_text::AsciiEncoding ascii;

}  // namespace nth::text_encoding

#endif  // NTH_STRINGS_TEXT_ENCODING_H
