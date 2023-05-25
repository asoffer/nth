package(default_visibility = ["//visibility:public"])

cc_library(
    name = "format",
    hdrs = ["format.h"],
    deps = [
        ":printer",
        ":universal_print",
        "//nth/meta:compile_time_string",
    ],
)

cc_test(
    name = "format_test",
    srcs = ["format_test.cc"],
    deps = [
        ":format",
        ":string_printer",
        "@com_google_googletest//:gtest_main",
    ],
)


cc_library(
    name = "printer",
    hdrs = ["printer.h"],
    deps = [],
)

cc_test(
    name = "printer_test",
    srcs = ["printer_test.cc"],
    deps = [
        ":printer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "string_printer",
    hdrs = ["string_printer.h"],
    deps = [],
)

cc_test(
    name = "string_printer_test",
    srcs = ["string_printer_test.cc"],
    deps = [
        ":string_printer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "universal_print",
    hdrs = ["universal_print.h"],
    deps = [
        ":printer",
        "//nth/meta:concepts",
        "//nth/meta:type",
        "//nth/utility:required",
    ],
)

cc_test(
    name = "universal_print_test",
    srcs = ["universal_print_test.cc"],
    deps = [
        ":string_printer",
        ":universal_print",
        "@com_google_googletest//:gtest_main",
    ],
)
