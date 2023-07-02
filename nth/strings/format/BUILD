package(default_visibility = ["//visibility:public"])

cc_library(
    name = "format",
    hdrs = ["format.h"],
    srcs = ["internal/format.h"],
    deps = [
        "//nth/io:printer",
        "//nth/meta:compile_time_string",
    ],
)

cc_test(
    name = "format_test",
    srcs = ["format_test.cc"],
    deps = [
        ":format",
        ":universal", # TODO: Cut this dep.
        "//nth/io:string_printer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "universal",
    hdrs = ["universal.h"],
    deps = [
        "//nth/io:printer",
        "//nth/meta:concepts",
        "//nth/meta:type",
        "//nth/utility:required",
    ],
)

cc_test(
    name = "universal_test",
    srcs = ["universal_test.cc"],
    deps = [
        ":format",
        ":universal",
        "//nth/io:string_printer",
        "@com_google_googletest//:gtest_main",
    ],
)
