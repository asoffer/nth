#ifndef NTH_DEBUG_CONTRACTS_INTERNAL_ANY_FORMATTABLE_REF_H
#define NTH_DEBUG_CONTRACTS_INTERNAL_ANY_FORMATTABLE_REF_H

#include <string>

#include "nth/format/format.h"
#include "nth/io/writer/writer.h"
#include "nth/io/writer/string.h"

namespace nth::internal_contracts {
struct any_formattable_ref {
  struct vtable {
    std::string (*format)(void const*);
  };

  template <typename T>
  static constexpr vtable vtable_for{
      .format =
          [](void const* raw_self) {
            std::string s;
            nth::io::string_writer w(s);
            nth::format(w, *reinterpret_cast<T const*>(raw_self));
            return s;
          },
  };

  template <typename T>
  constexpr any_formattable_ref(T const& t)
      : ptr_(nth::raw_address(t)), vtable_(&vtable_for<T>) {}

  friend void NthFormat(nth::io::writer auto& w, auto&,
                        any_formattable_ref ref) {
    nth::io::write_text(w, ref.vtable_->format(ref.ptr_));
  }

 private:
  void const* ptr_;
  vtable const* vtable_;
};

}  // namespace nth::internal_contracts

#endif  // NTH_DEBUG_CONTRACTS_INTERNAL_ANY_FORMATTABLE_REF_H
