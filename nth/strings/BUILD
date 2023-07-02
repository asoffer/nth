package(default_visibility = ["//visibility:public"])

cc_library(
    name = "edit_distance",
    hdrs = ["edit_distance.h"],
    srcs = ["edit_distance.cc"],
)

cc_test(
    name = "edit_distance_test",
    srcs = ["edit_distance_test.cc"],
    deps = [
        ":edit_distance",
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

cc_library(
    name = "text",
    hdrs = ["text.h"],
    srcs = ["text.cc"],
    deps = [
        ":text_encoding",
    ],
)

cc_test(
    name = "text_test",
    srcs = ["text_test.cc"],
    deps = [
        ":text",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "text_encoding",
    hdrs = ["text_encoding.h"],
    deps = [],
)
