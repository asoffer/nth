#ifndef NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H
#define NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H

#include <concepts>

namespace nth {

struct log_line_base;

namespace internal_log {

struct voidifier {
  template <typename T>
  friend void operator<<=(voidifier, T const&) {
    constexpr bool MissingInterpolationArguments =
        not std::derived_from<T, log_line_base>;
    // clang-format off
    static_assert(MissingInterpolationArguments,
        "\n\n"
        "    The log message requires interpolation arguments, but none were provided. It\n"
        "    requires interpolation arguments because an interpolation slot '{}' was used.\n"
        "    Interpolation arguments can be passed after the log message as shown in this\n"
        "    example:\n"
        "\n"
        "    ```\n"
        "    // Prints \"The sum of 2 and 3 is 5.\"\n"
        "    NTH_LOG(\"The sum of {} and {} is {}.\") <<= {2, 3, 5};\n"
        "    ```\n"
        "\n"
        "    If you wish to have braces treated literally rather than as interpolation\n"
        "    slots, you may escape them via an extra layer of braces:\n"
        "\n"
        "    ```\n"
        "    // Prints \"These are braces: {}, but these are not: not braces.\"\n"
        "    NTH_LOG(\"These are braces: {{}}, but these are not: {}\")\n"
        "        <<= {\"not braces\"};\n"
        "    ```\n");
    // clang-format on
  }
};

template <auto F>
struct invoking_voidifier : voidifier {
  ~invoking_voidifier() { F(); }
};

template <auto F>
struct non_returning_voidifier : voidifier {
  [[noreturn]] ~non_returning_voidifier() { F(); }
};

}  // namespace internal_log
}  // namespace nth

#endif  // NTH_DEBUG_LOG_INTERNAL_VOIDIFIER_H
