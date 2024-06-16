#ifndef NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
#define NTH_STRINGS_INTERPOLATE_INTERPOLATE_H

#include <type_traits>

#include "nth/io/format/interpolation_spec.h"
#include "nth/io/printer.h"
#include "nth/strings/interpolate/string.h"
#include "nth/strings/interpolate/types.h"

namespace nth {

template <int &..., io::printer_type P>
void interpolate(P &p, io::interpolation_spec spec, auto const &...values) {
  io::interpolation_spec child;
  ((p.write(spec.next_chunk(child)),
    (child.empty()
         ? (void)P::print(p,
                          nth::io::default_format_spec<
                              std::remove_cvref_t<decltype(values)>>(),
                          values)
         : (void)P::print(p, child, values))),
   ...);
  p.write(spec.last_chunk());
}

template <interpolation_string S, int &..., io::printer_type P>
void interpolate(P &p, auto const &...values) requires(sizeof...(values) ==
                                                       S.placeholders()) {
  interpolate(p, io::interpolation_spec::from<S>(), values...);
}

template <int &..., io::printer_type P>
void print_structure(P &p, io::interpolation_spec spec, auto const &...values) {
  io::interpolation_spec child;
  (((void)spec.next_chunk(child),
    (child.empty()
         ? (void)P::print(p,
                          nth::io::default_format_spec<
                              std::remove_cvref_t<decltype(values)>>(),
                          values)
         : (void)P::print(p, child, values))),
   ...);
}

template <interpolation_string S, int &..., io::printer_type P>
void print_structure(P &p, auto const &...values) requires(sizeof...(values) ==
                                                           S.placeholders()) {
  print_structure(p, io::interpolation_spec::from<S>(), values...);
}

}  // namespace nth

#endif  // NTH_STRINGS_INTERPOLATE_INTERPOLATE_H
