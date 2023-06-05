#include "nth/strings/internal/null_termination.h"

namespace nth::internal_null_termination {

char const *NullTerminated(NTH_ATTRIBUTE(lifetimebound) std::string const &s,
                           std::span<char> *) {
  return s.c_str();
}

char const *NullTerminated(NTH_ATTRIBUTE(lifetimebound)
                               std::same_as<char const *> auto s,
                           std::span<char> *) {
  return s;
}

StringDeleter NullTerminated(std::string_view s, std::span<char> *buffer) {
  if (buffer->size() < s.size()) {
    char *p = buffer->data();
    std::memcpy(p, s.data(), s.size());
    p[s.size()] = '\0';
    buffer += s.size();
    return StringDeleter::Unowned(p);
  } else {
    char *p = new char[s.size() + 1];
    std::memcpy(p, s.data(), s.size());
    p[s.size()] = '\0';
    return StringDeleter::Owned(p);
  }
}

}  // namespace nth::internal_null_termination
