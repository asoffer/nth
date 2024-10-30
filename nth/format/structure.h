#ifndef NTH_FORMAT_STRUCTURE_H
#define NTH_FORMAT_STRUCTURE_H

#include "nth/format/common.h"
#include "nth/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/meta/concepts/invocable.h"
#include "nth/types/structure.h"

namespace nth {

template <nth::io::writer W, typename F>
struct format_sequence_builder {
  explicit format_sequence_builder(W &w NTH_ATTRIBUTE(lifetimebound),
                                   F &fmt NTH_ATTRIBUTE(lifetimebound))
      : w_(w), fmt_(fmt) {
    nth::begin_format<nth::structure::sequence>(w_, fmt_);
  }

  template <typename T>
  void append_entry_with(auto &&fn) {
    nth::begin_format<nth::structure::entry>(w_, fmt_);
    NTH_FWD(fn)(w_, fmt_);
    nth::end_format<nth::structure::entry>(w_, fmt_);
  }

  template <typename T>
  void append_entry(T const &t) {
    nth::begin_format<nth::structure::entry>(w_, fmt_);
    nth::format(w_, fmt_, t);
    nth::end_format<nth::structure::entry>(w_, fmt_);
  }

  void append_entries(auto begin, auto end) {
    for (auto i = NTH_MOVE(begin); i != end; ++i) {
      nth::begin_format<nth::structure::entry>(w_, fmt_);
      nth::format(w_, fmt_, *i);
      nth::end_format<nth::structure::entry>(w_, fmt_);
    }
  }

  ~format_sequence_builder() {
    nth::end_format<nth::structure::sequence>(w_, fmt_);
  }

 private:
  W &w_;
  F &fmt_;
};

template <nth::io::writer W, typename F>
struct format_object_builder {
  explicit format_object_builder(W &w NTH_ATTRIBUTE(lifetimebound),
                                 F &fmt NTH_ATTRIBUTE(lifetimebound))
      : w_(w), fmt_(fmt) {
    nth::begin_format<nth::structure::object>(w_, fmt_);
  }

  void append_key_value(auto const &key, auto const &value) {
    nth::begin_format<nth::structure::key>(w_, fmt_);
    nth::format(w_, fmt_, key);
    nth::end_format<nth::structure::key>(w_, fmt_);
    nth::begin_format<nth::structure::value>(w_, fmt_);
    nth::format(w_, fmt_, value);
    nth::end_format<nth::structure::value>(w_, fmt_);
  }

  void append_key_with(auto const &key, auto &&value_fn) {
    nth::begin_format<nth::structure::key>(w_, fmt_);
    nth::format(w_, fmt_, key);
    nth::end_format<nth::structure::key>(w_, fmt_);
    nth::begin_format<nth::structure::value>(w_, fmt_);
    NTH_FWD(value_fn)(w_, fmt_);
    nth::end_format<nth::structure::value>(w_, fmt_);
  }

  ~format_object_builder() {
    nth::end_format<nth::structure::object>(w_, fmt_);
  }

 private:
  W &w_;
  F &fmt_;
};

}  // namespace nth

#endif  // NTH_FORMAT_STRUCTURE_H
