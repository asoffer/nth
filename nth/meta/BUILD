package(default_visibility = ["//visibility:public"])

cc_library(
    name = "compile_time_string",
    hdrs = ["compile_time_string.h"],
    deps = [],
)

cc_test(
    name = "compile_time_string_test",
    srcs = ["compile_time_string_test.cc"],
    deps = [
        ":compile_time_string",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "concepts",
    hdrs = ["concepts.h"],
    deps = [],
)

cc_test(
    name = "concepts_test",
    srcs = ["concepts_test.cc"],
    deps = [
        ":concepts",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "sequence",
    hdrs = ["sequence.h"],
    deps = [":type"],
)

cc_test(
    name = "sequence_test",
    srcs = ["sequence_test.cc"],
    deps = [
        ":sequence",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "type",
    hdrs = ["type.h"],
    deps = [],
)

cc_test(
    name = "type_test",
    srcs = ["type_test.cc"],
    deps = [
        ":type",
        "@com_google_googletest//:gtest_main",
    ],
)
