#ifndef NTH_META_STATEFUL_H
#define NTH_META_STATEFUL_H

#include "nth/base/macros.h"
#include "nth/meta/constant.h"
#include "nth/meta/sequence.h"

namespace nth::internal_stateful {

struct MutableSequenceBase {};

template <int N, auto List>
struct state {
  static constexpr int epoch = N;
  static constexpr auto list = List;
};

template <int N, typename Tag>
struct reader {
  friend auto constexpr state_func(reader<N, Tag>);
};

template <int N, auto List, typename Tag>
struct setter {
  friend auto constexpr state_func(reader<N, Tag>) { return List; }

  static constexpr state<N, List> state{};
};

template <auto>
struct MutableSequence : MutableSequenceBase {
 private:
  template <auto EvalTag, int N>
  [[nodiscard]] static consteval auto get_state() {
    if constexpr (requires(
                      nth::internal_stateful::reader<N, MutableSequence> r) {
                    state_func(r);
                  }) {
      return get_state<EvalTag, N + 1>();
    } else {
      constexpr nth::internal_stateful::reader<N - 1, MutableSequence> r;
      return nth::internal_stateful::state<N - 1, state_func(r)>{};
    }
  }

  template <auto X, auto EvalTag>
  static consteval auto append_impl() {
    using cur_state = decltype(get_state<EvalTag, 0>());
    nth::internal_stateful::setter<cur_state::epoch + 1,
                                   cur_state::list + nth::sequence<X>,
                                   MutableSequence>
        s;
    return s.state;
  }

 public:
  template <auto X, auto EvalTag = [] {},
            auto State = append_impl<X, EvalTag>()>
  constexpr void append() {}

  template <auto EvalTag = [] {}>
  constexpr auto operator*() const {
    constexpr auto list = decltype(get_state<EvalTag, 0>())::list;
    return list;
  }
};

}  // namespace nth::internal_stateful

// Defines a mutable compile-time sequence which can be appended to with the
// `append` member function and converted to an `nth::sequence` with
// `operator*`.
//
// Stateful metaprogramming is highly dangerous and should be avoided if at all
// possible.
#define NTH_DEFINE_MUTABLE_COMPILE_TIME_SEQUENCE(name)                         \
  NTH_REQUIRE_EXPANSION_IN_GLOBAL_NAMESPACE                                    \
  nth::internal_stateful::MutableSequence<[] {}> name;                         \
  template struct nth::internal_stateful::setter<0, nth::sequence<>,           \
                                                 decltype(name)>;              \
  static_assert(true)

#endif  // NTH_META_STATEFUL_H
