#ifndef NTH_STRINGS_INTERNAL_NULL_TERMINATION_H
#define NTH_STRINGS_INTERNAL_NULL_TERMINATION_H

#include <cstring>
#include <span>
#include <string>
#include <string_view>

#include "nth/base/attributes.h"

namespace nth::internal_null_termination {

// Stores a character pointer and a bool indication whether the data needs to be
// deleted.
struct StringDeleter {
  static constexpr StringDeleter Owned(char const *s) {
    return StringDeleter(s, true);
  }
  static constexpr StringDeleter Unowned(char const *s) {
    return StringDeleter(s, false);
  }

  constexpr operator char const *() const { return s_; }

  constexpr ~StringDeleter() {
    if (delete_) { delete[] s_; }
  }

 private:
  explicit constexpr StringDeleter(char const *s, bool d) : s_(s), delete_(d) {}

  char const *s_;
  bool delete_;
};

// The largest number of bytes to allocation on the stack as scratch space for
// copying temporary string data.
inline constexpr int MaxBufferSize = 1024;

char const *NullTerminated(NTH_ATTRIBUTE(lifetimebound) std::string const &s,
                           std::span<char> *);
char const *NullTerminated(NTH_ATTRIBUTE(lifetimebound)
                               std::same_as<char const *> auto s,
                           std::span<char> *) {
  return s;
}

StringDeleter NullTerminated(std::string_view s, std::span<char> *buffer);

template <int N>
StringDeleter NullTerminated(char const (&s)[N], std::span<char> *buffer) {
  if (s[N - 1] == '\0') {
    return StringDeleter::Unowned(s);
  } else if (buffer->size() < N) {
    char *p = buffer->data();
    std::memcpy(p, s, N);
    p[N]    = '\0';
    *buffer = buffer->subspan(N + 1);
    return StringDeleter::Unowned(p);
  } else {
    char *p = new char[N + 1];
    std::memcpy(p, s, N);
    p[N] = '\0';
    return StringDeleter::Owned(p);
  }
}

}  // namespace nth::internal_null_termination

#endif  // NTH_STRINGS_INTERNAL_NULL_TERMINATION_H
