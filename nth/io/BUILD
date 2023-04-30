package(default_visibility = ["//visibility:public"])

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
    name = "universal_print",
    hdrs = ["universal_print.h"],
    deps = [
        ":printer",
        "//nth/meta:concepts",
        "//nth/meta:type",
    ],
)


cc_test(
    name = "universal_print_test",
    srcs = ["universal_print_test.cc"],
    deps = [
        ":universal_print",
        "@com_google_googletest//:gtest_main",
    ],
)
