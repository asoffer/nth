#ifndef NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H
#define NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H

#include <concepts>

namespace nth::internal_debug{

struct Voidifier {
  template <typename T>
  friend void operator<<=(Voidifier, T const&) {
    constexpr bool MissingInterpolationArguments =
        not std::derived_from<T, struct LogLine>;
    // clang-format off
    static_assert(MissingInterpolationArguments,
        "\n\n"
        "    The log message requires interpolation arguments, but none were provided. It\n"
        "    requires interpolation arguments because an interpolation slot '{}' was used.\n"
        "    Interpolation arguments can be passed after the log message as shown in this\n"
        "    example:\n"
        "\n"
        "    ```\n"
        "    NTH_LOG(\"The sum of {} and {} is {}\") <<= {2, 3, 5};\n"
        "    ```\n"
        "\n"
        "    If you wish to have braces treated literally rather than as interpolation\n"
        "    slots, you may escape them via an extra layer of braces:\n"
        "\n"
        "    ```\n"
        "    NTH_LOG(\"These are braces: {{}}, but these are not: {}\")\n"
        "        <<= {\"not braces\"};\n"
        "    ```\n");
    // clang-format on
  }
};

}  // namespace nth::internal_debug

#endif  // NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H
