#ifndef NTH_IO_SERIALIZE_INTERNAL_SERIALIZE_H
#define NTH_IO_SERIALIZE_INTERNAL_SERIALIZE_H

#include <type_traits>

namespace nth::io::internal_serialize {

template <typename S>
struct serializer_result {
  using type = bool;
};

template <typename S>
requires requires { typename S::nth_serializer_result_type; }
struct serializer_result<S> {
  using type = S::nth_serializer_result_type;
  static_assert(std::is_convertible_v<type, bool>);
};

}  // namespace nth::io::internal_serialize

#endif  // NTH_IO_SERIALIZE_INTERNAL_SERIALIZE_H
