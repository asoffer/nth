#ifndef NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_INTERPOLATE_H

#include <type_traits>

#include "nth/base/core.h"
#include "nth/io/format/interpolation_spec.h"
#include "nth/io/writer/writer.h"
#include "nth/memory/bytes.h"
#include "nth/strings/interpolate/string.h"
#include "nth/strings/interpolate/types.h"

namespace nth {

template <int &..., io::forward_writer W>
void interpolate(W &w, io::interpolation_spec spec, auto const &...values) {
  io::interpolation_spec child;
  ((w.write(nth::byte_range(spec.next_chunk(child))),
    (child.empty()
         ? (void)W::format(w,
                           nth::io::default_format_spec<
                               std::remove_cvref_t<decltype(values)>>(),
                           values)
         : (void)W::format(w, child, values))),
   ...);
  w.write(nth::byte_range(spec.last_chunk()));
}

template <interpolation_string S, int &..., io::forward_writer W>
void interpolate(W &w, auto const &...values) requires(sizeof...(values) ==
                                                       S.placeholders()) {
  interpolate(w, io::interpolation_spec::from<S>(), values...);
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
