#ifndef NTH_META_COMPILE_TIME_STRING_H
#define NTH_META_COMPILE_TIME_STRING_H

#include <string_view>
#include <cstdlib>

namespace nth {

template <size_t Length>
struct CompileTimeString {
  constexpr CompileTimeString(char const (&buffer)[Length + 1]) {
    for (size_t i = 0; i < Length + 1; ++i) {
      implementation_data_[i] = buffer[i];
    }
  }

  size_t length() const { return Length; }
  size_t size() const { return Length; }

  constexpr operator char const*() const { return implementation_data_; }
  constexpr operator std::string_view() const { return implementation_data_; }

  char implementation_data_[Length + 1];
};

template <size_t N>
CompileTimeString(char const (&)[N]) -> CompileTimeString<N - 1>;

}  // namespace nth

#endif  // NTH_META_COMPILE_TIME_STRING_H
