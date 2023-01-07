package(default_visibility = ["//visibility:public"])

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
