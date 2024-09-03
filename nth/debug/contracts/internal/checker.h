#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H

#include "nth/debug/trace/internal/trace.h"
#include "nth/format/format.h"
#include "nth/format/interpolate.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/type.h"

namespace nth::internal_contracts {

template <typename T>
struct checker {
  explicit constexpr checker(T const& v) : result_(v) {}

  [[nodiscard]] constexpr bool ok() const { return result_; }

  [[nodiscard]] bool trace() const { return result_; }

  friend void NthFormat(io::writer auto& w, auto&, checker const& c) {
    nth::format(w, {}, c.result_);
  }

 private:
  bool result_;
};

template <std::derived_from<internal_trace::traced_expression_base> T>
struct checker<T> {
  explicit constexpr checker(T const& v) : result_(v) {}

  [[nodiscard]] constexpr bool ok() const {
    return internal_trace::traced_value(result_);
  }

  [[nodiscard]] T const& trace() const NTH_ATTRIBUTE(lifetimebound) {
    return result_;
  }

  friend void NthFormat(io::writer auto& w, auto&, checker const& c) {
    nth::format(w, {}, c.result_);
  }

 private:
  T const& result_;
};

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_CHECKER_H
