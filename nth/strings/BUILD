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
        "//nth/io:string_printer",
        "//nth/io:universal_print",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "null_termination",
    hdrs = ["null_termination.h"],
    srcs = [
        "internal/null_termination.h",
        "internal/null_termination.cc"
    ],
    deps = [
        "//nth/base:attributes",
        "//nth/meta:type",
    ],
)

cc_test(
    name = "null_termination_test",
    srcs = ["null_termination_test.cc"],
    deps = [
        ":null_termination",
        "@com_google_googletest//:gtest_main",
    ],
)