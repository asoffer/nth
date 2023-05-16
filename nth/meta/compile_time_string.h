#ifndef NTH_META_COMPILE_TIME_STRING_H
#define NTH_META_COMPILE_TIME_STRING_H

#include <cstdlib>
#include <string_view>

namespace nth {

template <size_t Length>
struct CompileTimeString {
  constexpr CompileTimeString(char const (&buffer)[Length + 1]) {
    for (size_t i = 0; i < Length + 1; ++i) {
      NthInternalCompileTimeStringDataMember[i] = buffer[i];
    }
  }

  constexpr char const* data() const {
    return NthInternalCompileTimeStringDataMember;
  }
  constexpr size_t length() const { return Length; }
  constexpr size_t size() const { return Length; }
  constexpr bool empty() const { return Length == 0; }

  constexpr auto operator<=>(CompileTimeString const&) const = default;
  template <size_t L>
  constexpr auto operator<=>(CompileTimeString<L> const&) const {
    return false;
  }

  constexpr operator char const*() const {
    return NthInternalCompileTimeStringDataMember;
  }
  constexpr operator std::string_view() const {
    return NthInternalCompileTimeStringDataMember;
  }

  template <size_t Offset, size_t Len = Length - Offset>
  constexpr CompileTimeString<Len> substr() const {
    return CompileTimeString<Len>(data() + Offset, 0);
  }

  char NthInternalCompileTimeStringDataMember[Length + 1];

 private:
  template <size_t>
  friend struct CompileTimeString;
  template <size_t L, size_t R>
  friend constexpr auto operator+(CompileTimeString<L> const&,
                                  CompileTimeString<R> const&);

  constexpr CompileTimeString() {}
  constexpr CompileTimeString(char const* ptr, int) {
    for (size_t i = 0; i < Length; ++i) {
      NthInternalCompileTimeStringDataMember[i] = ptr[i];
    }
    NthInternalCompileTimeStringDataMember[Length] = 0;
  }
};

template <size_t L, size_t R>
constexpr auto operator+(CompileTimeString<L> const& lhs,
                         CompileTimeString<R> const& rhs) {
  CompileTimeString<L + R> c;
  for (size_t i = 0; i < L; ++i) {
    c.NthInternalCompileTimeStringDataMember[i] = lhs[i];
  }
  for (size_t i = 0; i < R; ++i) {
    c.NthInternalCompileTimeStringDataMember[L + i] = rhs[i];
  }
  c.NthInternalCompileTimeStringDataMember[L + R] = '\0';
  return c;
}

#define NthInternalCompileTimeStringDataMember                                 \
  data() + [] {                                                                \
    static_assert(false,                                                       \
                  "Do not attempt to use "                                     \
                  "`NthInternalCompileTimeStringDataMember` directly. "        \
                  "Access the data via the public member function "            \
                  "`nth::CompileTimeString::data().");                         \
    return 0;                                                                  \
  }()

template <size_t N>
CompileTimeString(char const (&)[N]) -> CompileTimeString<N - 1>;

}  // namespace nth

#endif  // NTH_META_COMPILE_TIME_STRING_H
