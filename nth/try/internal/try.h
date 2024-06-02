#ifndef NTH_TRY_INTERNAL_TRY_H
#define NTH_TRY_INTERNAL_TRY_H

namespace nth::internal_try {

struct DefaultExitHandler {
  static constexpr bool okay(bool b) { return b; }

  template <typename T>
  static constexpr decltype(auto) transform_return(T&& value) {
    return std::forward<T>(value);
  }

  template <typename T>
  static constexpr decltype(auto) transform_value(T&& value) {
    return std::forward<T>(value);
  }
};

}  // namespace nth::internal_try

inline constexpr nth::internal_try::DefaultExitHandler
    NthInternalTryDefaultExitHandler;

#define NTH_TRY_INTERNAL_TRY(...)                                              \
  NTH_IF(NTH_IS_PARENTHESIZED(NTH_FIRST_ARGUMENT(__VA_ARGS__)),                \
         NTH_TRY_INTERNAL_TRY_WITH_HANDLER,                                    \
         NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER)                                 \
  (__VA_ARGS__)

#define NTH_TRY_INTERNAL_TRY_WITHOUT_HANDLER(...)                              \
  NTH_TRY_INTERNAL_TRY_WITH_HANDLER((NthInternalTryDefaultExitHandler),        \
                                    __VA_ARGS__)

#define NTH_TRY_INTERNAL_TRY_WITH_HANDLER(handler, ...)                        \
  ({                                                                           \
    auto&& expr                                      = (__VA_ARGS__);          \
    ::nth::try_exit_handler<decltype(expr)> auto&& h = handler;                \
    if (not h.okay(expr)) {                                                    \
      return h.transform_return(std::forward<decltype(expr)>(expr));           \
    }                                                                          \
    h.transform_value(std::forward<decltype(expr)>(expr));                     \
  })

#endif  // NTH_TRY_INTERNAL_TRY_H
