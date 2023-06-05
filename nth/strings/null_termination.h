#ifndef NTH_STRINGS_NULL_TERMINATION_H
#define NTH_STRINGS_NULL_TERMINATION_H

#include <concepts>
#include <span>
#include <string>
#include <string_view>

#include "nth/meta/sequence.h"
#include "nth/meta/type.h"
#include "nth/strings/internal/null_termination.h"

namespace nth {

// A concept matching any type that is used to to own or view string data.
// Specifically, this concept matches `std::string`, `std::string_view`, and
// arrays of pointers to `char` or `char const`.
template <typename T>
concept StringLike =
    (nth::type_sequence<char *, char const *, std::string, std::string_view>  //
         .template contains<nth::type<std::decay_t<T>>>());

// Invokes `f` on character pointers pointing to null-terminated strings
// which, during the invocation of `f`, compare equal to the corresponding
// values in the pack `std::string_view(ts)...`. The pointers passed to `f`
// point to valid null-terminated character strings during the invocation, but
// may be invalidated immediately after `f` returns.
template <int &..., StringLike... Ts,
          std::invocable<nth::first_t<char const *, Ts>...> F>
decltype(auto) InvokeOnNullTerminated(F &&f, Ts const &...ts) {
  if constexpr ((std::is_array_v<Ts> or ...) or
                ((nth::type<Ts> == nth::type<std::string_view>) or ...)) {
    char buffer[internal_null_termination::MaxBufferSize];
    std::span<char> span(std::begin(buffer), std::end(buffer));
    return std::apply(
        std::forward<F>(f),
        std::tuple{static_cast<char const *>(
            internal_null_termination::NullTerminated(ts, &span))...});
  } else {
    return std::forward<F>(f)(static_cast<char const *>(
        internal_null_termination::NullTerminated(ts, nullptr))...);
  }
}

}  // namespace nth

#endif  // NTH_STRINGS_NULL_TERMINATION_H
