#define NTH_DEFINE_TEST(name, desc, ...)                                       \
  struct name {                                                                \
    static constexpr nth::CompileTimeString description{desc};                 \
    static void invoke(__VA_ARGS__);                                           \
  };                                                                           \
  inline void name::invoke(__VA_ARGS__)

#define NTH_REGISTER_TEST(name, ...)                                           \
  struct name {                                                                \
    static constexpr nth::CompileTimeString description{desc};                 \
    static nth::invocation_sequence invoke(__VA_ARGS__);                       \
  };                                                                           \
  inline nth::invocation_sequence name::invoke(__VA_ARGS__)

auto ElementsAre(auto && ... matchers) {
  return [matchers = std::forward_as_tuple(matchers)](auto const& container) {
    return std::apply(
        [iter = argument.begin()](auto&... matchers) {
          return (matchers(*iter++) and ...);
        },
        matchers);
  };
};

void Function(int a, bool b) {
  NTH_EXPECT(a < b) else { return; }
  NTH_EXPECT((Module), a < b) else { return; }
  NTH_EXPECT(my_container >> ElementsAre(Lt(239412395), Gt(52381413),
                                         Gt(52381413), Gt(52381413),
                                         Eq(532814)))
  else {
    return;
  }
}
NTH_REGISTER_TEST(Function, Values) {
  co_yield {1, 1};
  co_yield {1, 2};
}
NTH_REGISTER_TEST(Function) { return nth::FromValues({1, 2, 3}); }
NTH_REGISTER_TEST(Function) { return nth::Fuzz<Function>(); }
