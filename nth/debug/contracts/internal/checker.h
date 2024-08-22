#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H

#include "nth/debug/trace/internal/trace.h"
#include "nth/format/format.h"
#include "nth/format/interpolate/interpolate.h"
#include "nth/format/interpolate/spec.h"
#include "nth/format/interpolate/string_view.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/type.h"

namespace nth::internal_contracts {

template <typename T>
struct checker {
  explicit constexpr checker(T const& v) : result_(v) {}

  [[nodiscard]] constexpr int get() const { return result_; }

  using nth_format_spec = nth::interpolation_spec;

  friend constexpr auto NthDefaultFormatSpec(nth::type_tag<checker>) {
    return nth::interpolation_spec::from<"{}">();
  }

  friend format_spec<checker> NthFormatSpec(interpolation_string_view s,
                                            type_tag<checker>) {
    return format_spec<checker>(s);
  }

  friend void NthFormat(io::forward_writer auto& w, format_spec<checker> spec,
                        checker const& c) {
    nth::interpolate(w, spec, c.result_);
  }

 private:
  bool result_;
};

template <std::derived_from<internal_trace::traced_expression_base> T>
struct checker<T> {
  explicit constexpr checker(T const& v) : result_(v) {}

  [[nodiscard]] constexpr int get() const { return internal_trace::traced_value(result_); }

  using nth_format_spec = nth::interpolation_spec;

  friend constexpr auto NthDefaultFormatSpec(nth::type_tag<checker>) {
    return nth::interpolation_spec::from<"{}">();
  }

  friend format_spec<checker> NthFormatSpec(interpolation_string_view s,
                                            type_tag<checker>) {
    return format_spec<checker>(s);
  }

  friend void NthFormat(io::forward_writer auto& w, format_spec<checker>,
                        checker const& c) {
    nth::format(w, {}, c.result_);
  }

 private:
  T const& result_;
};

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H
