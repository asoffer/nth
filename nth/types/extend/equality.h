#ifndef NTH_TYPES_EXTEND_EQUALITY_H
#define NTH_TYPES_EXTEND_EQUALITY_H

#include "nth/types/extend/extend.h"
#include "nth/types/reflect.h"

namespace nth::ext {

template <typename T>
struct equality : nth::extension<T> {
  friend bool operator==(T const &lhs, T const &rhs) {
    return nth::reflect::on_fields<1>(lhs, [&](auto const &...lhs_fields) {
      return nth::reflect::on_fields<1>(rhs, [&](auto const &...rhs_fields) {
        return ((lhs_fields == rhs_fields) and ...);
      });
    });
  }

  friend bool operator!=(T const &lhs, T const &rhs) { return not(lhs == rhs); }

  template <typename H>
  friend H AbslHashValue(H h, T const &value) {
    return nth::reflect::on_fields<1>(value, [&](auto const &...fields) {
      return H::combine(NTH_MOVE(h), fields...);
    });
  }
};

}  // namespace nth::ext

#endif  // NTH_TYPES_EXTEND_EQUALITY_H
