#ifndef NTH_IO_SERIALIZE_SEQUENCE_H
#define NTH_IO_SERIALIZE_SEQUENCE_H

#include <type_traits>

namespace nth::io {

// A proxy-likxe wrapper around sequneces that ensures is responsible for
// ensuring the underlying referenced object is serialized as a length-prefixed
// sequence.
template <typename>
struct as_sequence;

template <typename Seq>
requires requires(Seq seq) {
  { std::size(seq) } -> std::integral;
  std::begin(seq);
  std::end(seq);
}
struct as_sequence<Seq&> {
  using nth_lvalue_proxy = Seq&;

  explicit as_sequence(Seq& seq) : ref_(seq) {}

  Seq& ref() const { return ref_; }

 private:
  Seq& ref_;
};

template <typename Seq>
as_sequence(Seq&) -> as_sequence<Seq&>;
template <typename Seq>
as_sequence(Seq const&) -> as_sequence<Seq const&>;

}  // namespace nth::io

#endif  // NTH_IO_SERIALIZE_SEQUENCE_H
