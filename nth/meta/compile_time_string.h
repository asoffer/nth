#ifndef NTH_META_COMPILE_TIME_STRING_H
#define NTH_META_COMPILE_TIME_STRING_H

#include <compare>

#include "nth/meta/concepts/constructible.h"

namespace nth {

template <unsigned Length>
struct compile_time_string {
  constexpr compile_time_string(char const (&buffer)[Length + 1]) {
    for (unsigned i = 0; i < Length + 1; ++i) {
      NthInternalCompileTimeStringDataMember[i] = buffer[i];
    }
  }

  constexpr char const* data() const {
    return NthInternalCompileTimeStringDataMember;
  }
  constexpr unsigned length() const { return Length; }
  constexpr unsigned size() const { return Length; }
  constexpr bool empty() const { return Length == 0; }

  constexpr auto operator<=>(compile_time_string const&) const = default;

  template <unsigned R>
  requires(Length != R) friend constexpr bool operator==(
      compile_time_string const&, compile_time_string<R> const&) {
    return false;
  }

  template <unsigned R>
  requires(Length != R) friend constexpr bool operator!=(
      compile_time_string const&, compile_time_string<R> const&) {
    return true;
  }

  template <constructible_from<char (&)[Length + 1]> T>
  constexpr operator T() const {
    return T(NthInternalCompileTimeStringDataMember);
  }

  constexpr char operator[](unsigned n) const {
    return NthInternalCompileTimeStringDataMember[n];
  }

  template <unsigned Offset, unsigned Len = Length - Offset>
  constexpr compile_time_string<Len> substr() const {
    return compile_time_string<Len>(data() + Offset, 0);
  }

  char NthInternalCompileTimeStringDataMember[Length + 1];

  template <unsigned L, unsigned R>
  friend constexpr auto operator+(compile_time_string<L> const&,
                                  compile_time_string<R> const&);

 private:
  template <unsigned>
  friend struct compile_time_string;

  constexpr compile_time_string() {}
  constexpr compile_time_string(char const* ptr, int) {
    for (unsigned i = 0; i < Length; ++i) {
      NthInternalCompileTimeStringDataMember[i] = ptr[i];
    }
    NthInternalCompileTimeStringDataMember[Length] = 0;
  }
};

template <unsigned L, unsigned R>
constexpr auto operator+(compile_time_string<L> const& lhs,
                         compile_time_string<R> const& rhs) {
  compile_time_string<L + R> c;
  for (unsigned i = 0; i < L; ++i) {
    c.NthInternalCompileTimeStringDataMember[i] = lhs[i];
  }
  for (unsigned i = 0; i < R; ++i) {
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
                  "`nth::compile_time_string::data().");                       \
    return 0;                                                                  \
  }()

template <unsigned N>
compile_time_string(char const (&)[N]) -> compile_time_string<N - 1>;

}  // namespace nth

#endif  // NTH_META_COMPILE_TIME_STRING_H
