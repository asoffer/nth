#ifndef NTH_IO_DESERIALIZE_INTERNAL_DESERIALIZE_H
#define NTH_IO_DESERIALIZE_INTERNAL_DESERIALIZE_H

#include <type_traits>

namespace nth::io::internal_deserialize {

template <typename D>
struct deserializer_result {
  using type = bool;
};

template <typename D>
requires requires { typename D::nth_deserializer_result_type; }
struct deserializer_result<D> {
  using type = D::nth_deserializer_result_type;
  static_assert(std::is_convertible_v<type, bool>);
};

}  // namespace nth::io::internal_deserialize

#endif  // NTH_IO_DESERIALIZE_INTERNAL_DESERIALIZE_H
