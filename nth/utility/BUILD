package(default_visibility = ["//visibility:public"])

cc_library(
    name = "no_destructor",
    hdrs = ["no_destructor.h"],
    deps = [],
)

cc_test(
    name = "no_destructor_test",
    srcs = ["no_destructor_test.cc"],
    deps = [
        ":no_destructor",
        "@com_google_googletest//:gtest_main",
    ],
)
