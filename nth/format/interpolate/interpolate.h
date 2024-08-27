#ifndef NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
#define NTH_FORMAT_INTERPOLATE_INTERPOLATE_H

#include <type_traits>

#include "nth/base/core.h"
#include "nth/format/format.h"
#include "nth/format/interpolate/spec.h"
#include "nth/format/interpolate/string.h"
#include "nth/io/writer/writer.h"

namespace nth {

template <int &..., io::writer W>
void interpolate(W &w, interpolation_spec spec, auto const &...values) {
  interpolation_spec child;
  ((w.write(spec.next_chunk(child)),
    (void)format(
        w,
        child.empty()
            ? nth::default_format_spec<std::remove_cvref_t<decltype(values)>>()
            : format_spec_from<std::remove_cvref_t<decltype(values)>>(child),
        values)),
   ...);

  w.write(spec.last_chunk());
}

template <interpolation_string S, int &..., io::writer W>
void interpolate(W &w, auto const &...values)
  requires(sizeof...(values) == S.placeholders())
{
  interpolate(w, interpolation_spec::from<S>(), values...);
}

}  // namespace nth

#endif  // NTH_FORMAT_INTERPOLATE_INTERPOLATE_H
