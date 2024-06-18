#ifndef NTH_META_COMPILE_TIME_STRING_VIEW_H
#define NTH_META_COMPILE_TIME_STRING_VIEW_H

#include <cstddef>

namespace nth {

template <auto Predicate>
struct compile_time_string_view : std::string_view {
  template <size_t Length>
  consteval compile_time_string_view(char const (&buffer)[Length])
      : std::string_view(buffer, Length - 1) {
    if (not Predicate(std::string_view(buffer))) { std::abort(); }
  }
};

}  // namespace nth

#endif  // NTH_META_COMPILE_TIME_STRING_VIEW_H
