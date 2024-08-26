#ifndef NTH_TYPES_EXTEND_ORDER_H
#define NTH_TYPES_EXTEND_ORDER_H

#include "nth/types/extend/equality.h"
#include "nth/types/extend/extend.h"
#include "nth/types/reflect.h"

namespace nth {

template <typename T>
struct lexical_ordering : nth::extension<T, equality> {
  friend bool operator<=(T const &lhs, T const &rhs) {
    return nth::reflect::on_fields<1>(lhs, [&](auto const &...lhs_fields) {
      return nth::reflect::on_fields<1>(rhs, [&](auto const &...rhs_fields) {
        bool result = true;
        ((lhs_fields < rhs_fields    ? (result = true, false)
          : lhs_fields == rhs_fields ? true
                                     : result = false) and
         ...);
        return result;
      });
    });
  }

  friend bool operator>=(T const &lhs, T const &rhs) { return rhs <= lhs; }

  friend bool operator<(T const &lhs, T const &rhs) {
    return nth::reflect::on_fields<1>(lhs, [&](auto const &...lhs_fields) {
      return nth::reflect::on_fields<1>(rhs, [&](auto const &...rhs_fields) {
        bool result = false;
        ((lhs_fields < rhs_fields    ? (result = true, false)
          : lhs_fields == rhs_fields ? true
                                     : result = false) and
         ...);
        return result;
      });
    });
  }

  friend bool operator>(T const &lhs, T const &rhs) { return rhs < lhs; }
};

template <typename T>
struct product_ordering : nth::extension<T, equality> {
  friend bool operator<=(T const &lhs, T const &rhs) {
    return nth::reflect::on_fields<1>(lhs, [&](auto const &...lhs_fields) {
      return nth::reflect::on_fields<1>(rhs, [&](auto const &...rhs_fields) {
        return ((lhs_fields <= rhs_fields) and ...);
      });
    });
  }

  friend bool operator>=(T const &lhs, T const &rhs) { return rhs <= lhs; }

  friend bool operator<(T const &lhs, T const &rhs) {
    return lhs <= rhs and lhs != rhs;
  }

  friend bool operator>(T const &lhs, T const &rhs) { return rhs < lhs; }
};

}  // namespace nth

#endif  // NTH_TYPES_EXTEND_ORDER_H
